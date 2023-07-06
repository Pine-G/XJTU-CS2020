# 聊天程序服务器
import socket
import threading
import os
import datetime


class Server:
    def __init__(self, server_ip, server_port):
        self.client_sock = {}
        self.file_state = 0
        self.file_sent_size = 0
        self.user_file = "users.txt"
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 创建存储用户信息的文件
        if not os.path.isfile(self.user_file):
            with open(self.user_file, "w") as f:
                f.write("SERVER\n")
        # 将套接字绑定到指定的地址和端口
        self.socket.bind((server_ip, server_port))
        # 设置监听状态
        self.socket.listen(10)
        print(f"Server listening on {server_ip}: {server_port} ...")

    def start(self):
        while True:
            # 接受客户端连接请求
            client_socket, client_address = self.socket.accept()
            # 为每个客户端创建一个线程来处理请求
            threading.Thread(target=self.handle_client, args=(client_socket, )).start()

    def send_message(self, client_socket, message, data=b""):
        # 以二进制格式传输文件或语音
        if message.startswith("FILE_CONTENT") or message.startswith("VOIC_CONTENT"):
            parts = message.split("|", maxsplit=1)
            name = parts[1].encode("utf-8")    # 文件名或语音发送方
            sent_data = message[:12].encode("utf-8") + bytes([len(name)]) + name + data
        # 传输普通字符串
        else:
            sent_data = message.encode('utf-8')
        # 计算数据包长度封装消息头
        length = len(sent_data)
        header = bytes([length // 256, length % 256])
        try:
            # 发送消息头和消息体
            client_socket.sendall(header + sent_data)
        except socket.error:
            # 连接断开
            self.offline(client_socket)

    def receive_message(self, client_socket):
        try:
            message = b""
            # 先接收两个字节的消息头，得到消息长度
            header = client_socket.recv(2)
            try:
                length = header[0] * 256 + header[1]
            except IndexError:
                raise SystemExit
            # 根据消息长度接收消息体
            while length > len(message):
                message += client_socket.recv(length - len(message))
        except socket.error:
            # 连接断开
            self.offline(client_socket)
        else:
            # 接收文件或语音
            if message[:12].decode('utf-8') == "FILE_CONTENT" or message[:12].decode('utf-8') == "VOIC_CONTENT":
                name_len = message[12]
                recipient_len = message[13]
                name = message[14:14 + name_len].decode('utf-8')      # 文件名或语音发送方
                recipient = message[14 + name_len: 14 + name_len + recipient_len].decode('utf-8')
                return f"{message[:12].decode('utf-8')}|{name}|{recipient}", message[14 + name_len + recipient_len:]
            # 接收字符串
            else:
                return message.decode('utf-8'), b""

    # 用户离线
    def offline(self, client_socket):
        # 寻找套接字对应的用户名
        for username, sock in self.client_sock.items():
            if client_socket == sock:
                # 删除套接字
                del self.client_sock[username]
                # 服务器日志记录用户离线
                print(f"{username} leaves at {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
                # 向在线用户发送更新消息
                self.update_online_users(username, "DEL")
                break
        # 退出线程
        raise SystemExit

    # 更新在线用户列表
    def update_online_users(self, user, Op):
        # 向已在线用户广播更新消息
        for username, sock in self.client_sock.items():
            self.send_message(sock, f"UPDATE_USERS|{Op}|{user}")
        # 向新上线用户发送在线用户列表
        if Op == "ADD":
            for username, sock in self.client_sock.items():
                if user != username:
                    self.send_message(self.client_sock[user], f"UPDATE_USERS|{Op}|{username}")

    # 登录认证
    def login(self, client_socket, message):
        parts = message.split("|", maxsplit=1)
        username = parts[0]
        password = parts[1]
        with open(self.user_file, "r") as f:
            f.seek(0)
            lines = f.readlines()
            found = False
            # 查询用户是否存在
            for line in lines:
                parts = line.strip().split()
                if len(parts) == 2 and parts[0] == username:
                    found = True
                    # 判断密码是否正确
                    if parts[1] == password:
                        # 记录用户套接字
                        self.client_sock[username] = client_socket
                        # 服务器日志记录用户登录
                        print(f"{username} login at {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
                        # 向客户端发送登录成功消息
                        self.send_message(client_socket, f"LOGIN_SUCCEED")
                        # 更新在线用户列表
                        self.update_online_users(username, "ADD")
                        # 查询是否有离线文件需要发送给该客户端
                        threading.Thread(target=self.offline_file).start()
                    else:
                        # 账号与密码不匹配
                        self.send_message(client_socket, f"LOGIN_FAIL")
                    break
            if not found:
                # 用户不存在
                self.send_message(client_socket, f"LOGIN_FAIL")

    # 注册认证
    def register(self, client_socket, message):
        parts = message.split("|", maxsplit=1)
        username = parts[0]
        password = parts[1]
        with open(self.user_file, "a+") as f:
            f.seek(0)
            lines = f.readlines()
            found = False
            # 查询用户是否存在
            for line in lines:
                parts = line.strip().split()
                if parts[0] == username:
                    # 账号名已存在
                    found = True
                    self.send_message(client_socket, f"REGISTER_FAIL")
                    break
            # 用户不存在则创建新用户
            if not found:
                # 服务器记录新账号与密码
                f.write(f"{username} {password}\n")
                # 记录用户套接字
                self.client_sock[username] = client_socket
                # 服务器日志记录用户注册
                print(f"{username} register at {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
                # 向客户端发送注册成功消息
                self.send_message(client_socket, f"REGISTER_SUCCEED")
                # 更新在线用户列表
                self.update_online_users(username, "ADD")

    # 发送私聊消息
    def private_message(self, client_socket, message):
        parts = message.split("|", maxsplit=2)
        sender = parts[0]
        recipient = parts[1]
        message = parts[2]
        if recipient in self.client_sock:
            recipient_socket = self.client_sock[recipient]
            if recipient_socket != client_socket:
                self.send_message(recipient_socket, f"PRIVATE|{sender}|{recipient}|{message}")

    # 发送群聊消息
    def public_message(self, client_socket, message):
        parts = message.split("|", maxsplit=1)
        sender = parts[0]
        message = parts[1]
        for username, sock in self.client_sock.items():
            if sock != client_socket:
                self.send_message(sock, f"GROUP|{sender}|{message}")

    # 发送文件
    def send_file(self, client_socket, message, data):
        # 处理发送文件请求
        if message.startswith("HEADER"):
            parts = message.split("|", maxsplit=4)
            sender = parts[1]
            recipient = parts[2]
            file_name = parts[3]
            file_size = parts[4]
            # 查看接收方是否存在
            with open(self.user_file, "r") as f:
                f.seek(0)
                lines = f.readlines()
                found = False
                for line in lines:
                    parts = line.strip().split()
                    if parts[0] == recipient:
                        found = True
                        if recipient in self.client_sock:
                            # 接收方在线
                            self.send_message(self.client_sock[recipient], f"FILE_HEADER|{sender}|{file_name}|{file_size}")
                        else:
                            # 接收方离线
                            file_sent_size = 0
                            # 查看服务器是否存在发送未完成的临时文件
                            if os.path.isfile(file_name+".tmp"):
                                file_sent_size = os.path.getsize(file_name+".tmp")
                            # 通知客户端发送离线文件
                            self.send_message(client_socket, f"FILE_OFFLINE_USER|{file_sent_size}")
                        break
                if not found:
                    # 接收方不存在
                    self.send_message(client_socket, "FILE_USER_NO_EXIST")
        # 发送文件内容
        elif message.startswith("CONTENT"):
            parts = message.split("|", maxsplit=2)
            file_name = parts[1]
            recipient = parts[2]
            if recipient == "SERVER":
                # 服务器本地存储离线文件
                with open(file_name + ".tmp", "ab") as f:
                    f.write(data)
            else:
                # 向接收方转发在线文件
                self.send_message(self.client_sock[recipient], f"FILE_CONTENT|{file_name}", data)
        # 文件发送完成
        elif message.startswith("END"):
            parts = message.split("|", maxsplit=5)
            sender = parts[1]
            recipient = parts[2]
            file_name = parts[3]
            file_size = parts[4]
            target = parts[5]
            if recipient == "SERVER":
                # 服务器日志记录离线文件任务
                os.rename(file_name + ".tmp", file_name)
                with open("offline_file.txt", "ab") as f:
                    f.write(f"{sender}|{target}|{file_name}|{file_size}|_READY\n".encode("utf-8"))
                threading.Thread(target=self.offline_file).start()
            else:
                # 通知接收方文件发送完成
                self.send_message(self.client_sock[recipient], f"FILE_END|{file_name}")
        # 文件发送取消
        elif message.startswith("CANCEL"):
            parts = message.split("|", maxsplit=2)
            recipient = parts[1]
            file_name = parts[2]
            if recipient != "SERVER":
                self.send_message(self.client_sock[recipient], f"FILE_CANCEL|{file_name}")
        # 对方拒绝接收文件
        elif message.startswith("REJECT"):
            parts = message.split("|", maxsplit=2)
            recipient = parts[2]
            if recipient == "SERVER":
                self.file_state = 1
            else:
                self.send_message(self.client_sock[recipient], f"FILE_REJECT")
        # 对方同意接收文件
        elif message.startswith("ACCEPT"):
            parts = message.split("|", maxsplit=3)
            recipient = parts[2]
            file_sent_size = parts[3]
            if recipient == "SERVER":
                self.file_sent_size = int(file_sent_size)
                self.file_state = 2
            else:
                self.send_message(self.client_sock[recipient], f"FILE_ACCEPT|{file_sent_size}")

    # 处理离线文件
    def offline_file(self):
        if os.path.isfile("offline_file.txt"):
            # 查询日志
            with open("offline_file.txt", "rb+") as f:
                f.seek(0)
                line = f.readline().decode("utf-8").rstrip()
                while line:
                    if line.endswith("FINISH"):
                        # 离线文件传输已完成
                        line = f.readline().decode("utf-8").rstrip()
                        continue
                    parts = line.split("|")
                    sender = parts[0]
                    recipient = parts[1]
                    file_name = parts[2]
                    file_size = parts[3]
                    if recipient not in self.client_sock:
                        # 接收方离线
                        line = f.readline().decode("utf-8").rstrip()
                        continue
                    # 接收方上线后，询问是否接收文件
                    self.send_message(self.client_sock[recipient], f"FILE_OFFLINE_HEADER|{sender}|{file_name}|{file_size}")
                    # 等待应答
                    while not self.file_state:
                        continue
                    # 发送离线文件
                    if self.file_state == 2:
                        message = f"FILE_CONTENT|{file_name}"
                        with open(file_name, "rb") as f1:
                            # 定位到上次文件传输中断的位置
                            f1.seek(self.file_sent_size)
                            total_sent = self.file_sent_size
                            while total_sent < int(file_size):
                                data = f1.read(2 ** 15)
                                total_sent += len(data)
                                self.send_message(self.client_sock[recipient], message, data)
                        self.file_sent_size = 0
                        self.send_message(self.client_sock[recipient], f"FILE_END|{file_name}")
                    # 服务器本地删除文件
                    os.remove(file_name)
                    # 服务器日志记录离线传输完成
                    f.seek(-7, 1)
                    f.write("FINISH\n".encode("utf-8"))
                    self.file_state = 0
                    # 读取下一条记录
                    line = f.readline().decode("utf-8").rstrip()

    # 发送语音
    def send_voice(self, message, data):
        # 处理发送语音请求
        if message.startswith("HEADER"):
            parts = message.split("|", maxsplit=2)
            sender = parts[1]
            recipient = parts[2]
            self.send_message(self.client_sock[recipient], f"VOIC_HEADER|{sender}")
        # 发送语音内容
        elif message.startswith("CONTENT"):
            parts = message.split("|", maxsplit=2)
            sender = parts[1]
            recipient = parts[2]
            self.send_message(self.client_sock[recipient], f"VOIC_CONTENT|{sender}", data)
        # 关闭语音通话
        elif message.startswith("END"):
            parts = message.split("|", maxsplit=2)
            sender = parts[1]
            recipient = parts[2]
            self.send_message(self.client_sock[recipient], f"VOIC_END|{sender}")
        # 对方拒绝接收语音
        elif message.startswith("REJECT"):
            parts = message.split("|", maxsplit=2)
            recipient = parts[2]
            self.send_message(self.client_sock[recipient], f"VOIC_REJECT")
        # 对方同意接收语音
        elif message.startswith("ACCEPT"):
            parts = message.split("|", maxsplit=2)
            recipient = parts[2]
            self.send_message(self.client_sock[recipient], f"VOIC_ACCEPT")

    # NAT穿透（全锥形）
    def nat(self, message):
        # NAT穿透请求
        if message.startswith("REQUEST"):
            parts = message.split("|", maxsplit=2)
            sender = parts[1]
            recipient = parts[2]
            self.send_message(self.client_sock[recipient], f"NAT_REQUEST|{sender}")
        # 同意NAT穿透
        elif message.startswith("ACCEPT"):
            parts = message.split("|", maxsplit=2)
            sender = parts[1]
            recipient = parts[2]
            self.send_message(self.client_sock[recipient], f"NAT_ACCEPT|{sender}")
            # 创建线程以获取客户端公网ip地址与端口号
            threading.Thread(target=self.nat_thread).start()
        # 拒绝NAT穿透
        elif message.startswith("REJECT"):
            parts = message.split("|", maxsplit=2)
            recipient = parts[2]
            self.send_message(self.client_sock[recipient], "NAT_REJECT")

    def nat_thread(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("172.18.152.17", 6666))
        for i in range(2):
            # 将客户端公网ip地址与端口号转发给对方
            message, address = sock.recvfrom(1024)
            message = message.decode("utf-8")
            parts = message.split("|", maxsplit=2)
            recipient = parts[1]
            self.send_message(self.client_sock[recipient], f"NAT_ADDRESS|{address}")
        sock.close()

    # 接收客户端消息
    def handle_client(self, client_socket):
        while True:
            message, data = self.receive_message(client_socket)
            if message.startswith("LOGIN"):
                self.login(client_socket, message[6:])
            elif message.startswith("REGISTER"):
                self.register(client_socket, message[9:])
            elif message.startswith("PRIVATE"):
                self.private_message(client_socket, message[8:])
            elif message.startswith("GROUP"):
                self.public_message(client_socket, message[6:])
            elif message.startswith("FILE"):
                self.send_file(client_socket, message[5:], data)
            elif message.startswith("VOIC"):
                self.send_voice(message[5:], data)
            elif message.startswith("NAT"):
                self.nat(message[4:])


if __name__ == "__main__":
    Server_ip = "172.18.152.17"     # 服务器私网ip地址
    Server_port = 8888

    server = Server(Server_ip, Server_port)
    server.start()
