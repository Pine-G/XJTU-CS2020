from ryu.base import app_manager
from ryu.base.app_manager import lookup_service_brick
from ryu.controller import ofp_event
from ryu.controller.handler import MAIN_DISPATCHER, CONFIG_DISPATCHER
from ryu.controller.handler import set_ev_cls
from ryu.ofproto import ofproto_v1_0
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet, arp, ipv4

from network_awareness import NetworkAwareness
import requests
import json

class ShortestPath(app_manager.RyuApp):
    OFP_VERSIONS = [ofproto_v1_0.OFP_VERSION]

    _CONTEXTS = {
        'network_awareness': NetworkAwareness
    }

    def __init__(self, *args, **kwargs):
        super(ShortestPath, self).__init__(*args, **kwargs)
        self.network_awareness = kwargs['network_awareness']
        self.dpid_mac_port = {}

    def send_flow_mod(self, parser, dpid, pkt_type, src_ip, dst_ip, in_port, out_port, priority=1):
        flow = {
            "dpid": dpid,
            "idle_timeout": 0,
            "hard_timeout": 0,
            "priority": priority,
            "match":{
                "dl_type": 2048,
                "in_port": in_port,
                "nw_src": src_ip,
                "nw_dst": dst_ip
            },
            "actions":[
                {
                    "type":"OUTPUT",
                    "port": out_port
                }
            ]
        }

        url = 'http://localhost:8080/stats/flowentry/add'
        ret = requests.post(
            url, headers={'Accept': 'application/json', 'Accept': 'application/json'}, data=json.dumps(flow))
        print(ret)

    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def packet_in_handler(self, ev):
        msg = ev.msg
        dp = msg.datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser

        dpid = dp.id
        in_port = msg.in_port

        pkt = packet.Packet(msg.data)
        eth_pkt = pkt.get_protocol(ethernet.ethernet)
        arp_pkt = pkt.get_protocol(arp.arp)
        ipv4_pkt = pkt.get_protocol(ipv4.ipv4)

        pkt_type = eth_pkt.ethertype

        # layer 2 self-learning
        dst_mac = eth_pkt.dst
        src_mac = eth_pkt.src

        self.dpid_mac_port.setdefault(dpid, {})
        self.dpid_mac_port[dpid][src_mac] = in_port

        if isinstance(arp_pkt, arp.arp):
            self.handle_arp(msg, in_port, dst_mac, pkt_type)

        if isinstance(ipv4_pkt, ipv4.ipv4):
            self.handle_ipv4(msg, ipv4_pkt.src, ipv4_pkt.dst, pkt_type)

    def handle_arp(self, msg, in_port, dst_mac, pkt_type):
        dp = msg.datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser

        dpid = dp.id

        if dst_mac in self.dpid_mac_port[dpid]:
            out_port = self.dpid_mac_port[dpid][dst_mac]
            actions = [parser.OFPActionOutput(out_port)]
            out = parser.OFPPacketOut(
                datapath=dp, buffer_id=msg.buffer_id, in_port=in_port, actions=actions, data=msg.data)
            dp.send_msg(out)
        else:
            # send to the switch port which linked hosts
            for d, ports in self.network_awareness.port_info.items():
                for p in ports:
                    # except the source host
                    if d == dpid and p == in_port:
                        continue
                    dp = self.network_awareness.switch_info[d]
                    actions = [parser.OFPActionOutput(p)]
                    out = parser.OFPPacketOut(
                        datapath=dp, buffer_id=msg.buffer_id, in_port=ofp.OFPP_CONTROLLER, actions=actions, data=msg.data)
                    dp.send_msg(out)

    def handle_ipv4(self, msg, src_ip, dst_ip, pkt_type):
        parser = msg.datapath.ofproto_parser

        dpid_path = self.network_awareness.shortest_path(src_ip, dst_ip)
        if not dpid_path:
            return

        port_path = []
        for i in range(1, len(dpid_path) - 1):
            in_port = self.network_awareness.link_info[(dpid_path[i], dpid_path[i - 1])]
            out_port = self.network_awareness.link_info[(dpid_path[i], dpid_path[i + 1])]
            port_path.append((in_port, dpid_path[i], out_port))
        self.show_path(src_ip, dst_ip, port_path)

        # send flow mod
        for node in port_path:
            in_port, dpid, out_port = node
            self.send_flow_mod(parser, dpid, pkt_type, "10.0.0.0/24", "10.0.0.0/24", in_port, out_port)
            self.send_flow_mod(parser, dpid, pkt_type, "10.0.0.0/24", "10.0.0.0/24", out_port, in_port)

        # send packet_out
        _, dpid, out_port = port_path[-1]
        dp = self.network_awareness.switch_info[dpid]
        actions = [parser.OFPActionOutput(out_port)]
        out = parser.OFPPacketOut(
            datapath=dp, buffer_id=msg.buffer_id, in_port=in_port, actions=actions, data=msg.data)
        dp.send_msg(out)

    def show_path(self, src, dst, port_path):
        self.logger.info('path: {} -> {}'.format(src, dst))
        path = src + ' -> '
        for node in port_path:
            path += '{}:s{}:{}'.format(*node) + ' -> '
        path += dst
        self.logger.info(path)
        self.logger.info('\n')
