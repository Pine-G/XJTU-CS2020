from mininet.topo import Topo
from mininet.net import Mininet
from mininet.cli import CLI
from mininet.log import setLogLevel
import os

class FatTree(Topo):

    def __init__(self, k, **opts):

        super(FatTree, self).__init__(**opts)

        CoreSwitches = [self.addSwitch('CS'+str(i+1)) for i in range((k/2)**2)]
        AggrSwitches = [self.addSwitch('AS'+str(i+1)) for i in range((k/2)*k)]
        EdgeSwitches = [self.addSwitch('ES'+str(i+1)) for i in range((k/2)*k)]
        Host = [self.addHost('H'+str(i+1)) for i in range((k**3)/4)]

        # Create Link between Core and Aggregation
        for i in range((k/2)**2):
            for j in range(k):
                self.addLink(CoreSwitches[i], AggrSwitches[i//(k/2)+j*(k/2)])
        
        # Create Link between Aggregation and Edge
        for i in range((k/2)*k):
            for j in range(k/2):
                self.addLink(AggrSwitches[i], EdgeSwitches[i//(k/2)*(k/2)+j])
        
        # Create Link between Edge and Host
        for i in range((k/2)*k):
            for j in range(k/2):
                self.addLink(EdgeSwitches[i], Host[i*(k/2)+j])
        

def run(k=2):
    topo = FatTree(k)
    net = Mininet(topo=topo, controller=None, autoSetMacs=True)

    net.start()

    for i in range((k/2)**2):
        os.system('sudo ovs-vsctl set bridge CS' + str(i+1) + ' stp_enable=true')
        os.system('sudo ovs-vsctl del-fail-mode CS' + str(i+1))
    for i in range((k/2)*k):
        os.system('sudo ovs-vsctl set bridge AS' + str(i+1) + ' stp_enable=true')
        os.system('sudo ovs-vsctl del-fail-mode AS' + str(i+1))
    for i in range((k/2)*k):
        os.system('sudo ovs-vsctl set bridge ES' + str(i+1) + ' stp_enable=true')
        os.system('sudo ovs-vsctl del-fail-mode ES' + str(i+1))

    CLI(net)
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    run(4)