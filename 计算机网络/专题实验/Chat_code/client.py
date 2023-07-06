# 聊天程序客户端
from tkinter import messagebox
from tkinter import scrolledtext
from tkinter import filedialog
from tkinter import simpledialog
import tkinter as tk
import socket
import threading
import os
import time
import datetime
import pyaudio

#############################################################################


class Client:
    def __init__(self, server_ip, server_port):
        self.username = ""
        self.is_connected = False
        self.close_window = False
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            # 建立连接
            self.socket.connect((server_ip, server_port))
        except socket.error as e:
            # 连接失败
            self.socket.close()
            tk.Tk().withdraw()
            messagebox.showerror('错误', str(e))
            raise SystemExit
        else:
            # 连接成功
            self.is_connected = True

    def send_message(self, message, data=b""):
        # 以二进制格式传输文件或语音
        if message.startswith("FILE_CONTENT") or message.startswith("VOIC_CONTENT"):
            parts = message.split("|", maxsplit=2)
            name = parts[1].encode("utf-8")     # 文件名或语音发送方
            recipient = parts[2].encode("utf-8")
            sent_data = message[:12].encode("utf-8") + bytes([len(name), len(recipient)]) + name + recipient + data
        # 传输普通字符串
        else:
            sent_data = message.encode('utf-8')
        # 计算数据包长度封装消息头
        length = len(sent_data)
        header = bytes([length // 256, length % 256])
        try:
            # 发送消息头和消息体
            self.socket.sendall(header + sent_data)
        except socket.error as e:
            # 连接断开
            if self.is_connected:
                self.is_connected = False
                self.socket.close()
                messagebox.showerror("错误", str(e))
                self.close_window = True
            raise SystemExit

    def receive_message(self):
        try:
            message = b""
            # 先接收两个字节的消息头，得到消息长度
            header = self.socket.recv(2)
            length = header[0] * 256 + header[1]
            # 根据消息长度接收消息体
            while length > len(message):
                message += self.socket.recv(length - len(message))
        except socket.error as e:
            # 连接断开
            if self.is_connected:
                self.is_connected = False
                self.socket.close()
                messagebox.showerror("错误", str(e))
                self.close_window = True
            raise SystemExit
        else:
            # 接收文件或语音
            if message[:12].decode('utf-8') == "FILE_CONTENT" or message[:12].decode('utf-8') == "VOIC_CONTENT":
                name_len = message[12]
                name = message[13:13 + name_len].decode('utf-8')    # 文件名或语音发送方
                return f"{message[:12].decode('utf-8')}|{name}", message[13 + name_len:]
            # 接收字符串
            else:
                return message.decode('utf-8'), b""

#############################################################################


class LoginWindow:
    def __init__(self, _client):
        self.client = _client
        # 主窗口
        self.window = tk.Tk()
        self.window.geometry("450x300")
        self.window.title("聊天室")
        self.window.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.window.after(1000, self.check_connect)
        # 用户名标签
        self.label_username = tk.Label(self.window, text="用户名", font=20)
        self.label_username.place(x=60, y=50)
        # 用户名输入框
        self.entry_username = tk.Entry(self.window, font=("Helvetica", 14))
        self.entry_username.place(x=150, y=50)
        # 密码标签
        self.label_password = tk.Label(self.window, text="密码", font=20)
        self.label_password.place(x=60, y=100)
        # 密码输入框
        self.entry_password = tk.Entry(self.window, font=("Helvetica", 14), show='*')
        self.entry_password.place(x=150, y=100)
        # 登录按钮
        self.button_login = tk.Button(self.window, text="登录", font=25, command=self.login)
        self.button_login.place(x=150, y=180)
        # 注册按钮
        self.button_register = tk.Button(self.window, text="注册", font=25, command=self.register)
        self.button_register.place(x=250, y=180)

        self.window.mainloop()

    def on_closing(self):
        self.client.socket.close()
        self.window.destroy()
        raise SystemExit

    def check_connect(self):
        if self.client.close_window:
            self.window.destroy()
            raise SystemExit
        else:
            # 每隔1秒检测连接是否断开
            self.window.after(1000, self.check_connect)

    def login(self):
        self.client.username = self.entry_username.get()
        # 判断用户名是否合法
        if self.client.username.isalnum() and 2 < len(self.client.username) < 10:
            password = self.entry_password.get()
            # 判断密码是否合法
            if password.isalnum() and 5 < len(password) < 10:
                # 发送登录认证
                self.client.send_message(f"LOGIN|{self.client.username}|{password}")
                # 等待服务器发送登录确认
                message, data = self.client.receive_message()
                if message.startswith("LOGIN_SUCCEED"):
                    self.window.destroy()
                elif message.startswith("LOGIN_FAIL"):
                    messagebox.showerror('错误', "账号或密码错误")
            else:
                messagebox.showerror('错误', "密码只能含有数字或大小写字母且长度6~9")
        else:
            messagebox.showerror('错误', "账号只能含有数字或大小写字母且长度3~9")

    def register(self):
        self.client.username = self.entry_username.get()
        # 判断用户名是否合法
        if self.client.username.isalnum() and 2 < len(self.client.username) < 10:
            password = self.entry_password.get()
            # 判断密码是否合法
            if password.isalnum() and 5 < len(password) < 10:
                # 发送注册请求
                self.client.send_message(f"REGISTER|{self.client.username}|{password}")
                # 等待服务器发送注册确认
                message, data = self.client.receive_message()
                if message.startswith("REGISTER_SUCCEED"):
                    self.window.destroy()
                elif message.startswith("REGISTER_FAIL"):
                    messagebox.showerror('错误', "该账号已存在")
            else:
                messagebox.showerror('错误', "密码只能含有数字或大小写字母且长度6~9")
        else:
            messagebox.showerror('错误', "账号只能含有数字或大小写字母且长度3~9")

#############################################################################


class ChatWindow:
    def __init__(self, _client):
        self.client = _client
        self.file_state = 0
        self.file_sent_size = 0
        self.voice_state = 0
        self.MyPeer = None
        self.P2P = False
        # 主窗口
        self.window = tk.Tk()
        self.window.geometry("560x480")
        self.window.title(f"聊天室 - {self.client.username}")
        self.window.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.window.after(1000, self.check_connect)
        # 消息显示界面
        self.textbox = scrolledtext.ScrolledText(self.window, font=("Helvetica", 14), width=32, height=15)
        self.textbox.place(x=5, y=5)
        self.textbox.bind("<Key>", self.disable_keyboard)
        # 在线用户列表选择框
        self.user_list = tk.Listbox(self.window, font=("Helvetica", 14), selectmode="single", width=15, height=12)
        self.user_list.place(x=380, y=35)
        # 消息输入框
        self.input = tk.Text(self.window, font=("Helvetica", 14), width=26, height=4)
        self.input.place(x=75, y=365)
        # 在线用户列表标签
        self.label1 = tk.Label(self.window, text="在线用户列表", font=10)
        self.label1.place(x=380, y=5)
        # 输入标签
        self.label2 = tk.Label(self.window, text="输入：", font=10)
        self.label2.place(x=6, y=400)
        # 私聊按钮
        self.button1 = tk.Button(self.window, text="私聊", font=10, command=self.send_private_message)
        self.button1.place(x=400, y=320)
        # 群聊按钮
        self.button2 = tk.Button(self.window, text="群聊", font=10, command=self.send_group_message)
        self.button2.place(x=460, y=320)
        # 发送文件按钮
        self.button3 = tk.Button(self.window, text="文件", font=10, command=self.send_file)
        self.button3.place(x=400, y=360)
        # 语音通话按钮
        self.button4 = tk.Button(self.window, text="语音", font=10, command=self.send_voice)
        self.button4.place(x=460, y=360)
        # NAT穿透按钮
        self.button5 = tk.Button(self.window, text="NAT穿透", font=10, command=self.nat_request)
        self.button5.place(x=400, y=400)
        # P2P私聊按钮
        self.button6 = tk.Button(self.window, text="P2P私聊", font=10, command=self.P2P_chat)
        self.button6.place(x=400, y=440)

        # 语音通话参数
        chunk_size = 1024                   # 设置每个音频缓冲区的大小为1024帧
        audio_format = pyaudio.paInt16      # 设置音频格式为16位整型
        channels = 1                        # 设置音频通道数为1（单声道）
        rate = 20000                        # 设置音频采样率为20000Hz
        # 播放音频的音频流对象
        self.playing_stream = pyaudio.PyAudio().open(format=audio_format, channels=channels, rate=rate, output=True,
                                                     frames_per_buffer=chunk_size)
        # 录制音频的音频流对象
        self.recording_stream = pyaudio.PyAudio().open(format=audio_format, channels=channels, rate=rate, input=True,
                                                       frames_per_buffer=chunk_size)

        # 创建线程用来接收服务器发来的消息
        self.receive_thread = threading.Thread(target=self.receive_messages)
        self.receive_thread.setDaemon(True)
        self.receive_thread.start()

        self.window.mainloop()

    def on_closing(self):
        self.client.socket.close()
        self.window.destroy()
        raise SystemExit

    def check_connect(self):
        if self.client.close_window:
            self.window.destroy()
            raise SystemExit
        else:
            # 每隔1秒检测连接是否断开
            self.window.after(1000, self.check_connect)

    # 消息显示界面禁止键盘输入
    @staticmethod
    def disable_keyboard(event):
        return "break"

    # 发送私聊消息
    def send_private_message(self):
        # 获取选中的私聊用户
        recipient = self.user_list.get("anchor")
        if recipient:
            message = self.input.get("1.0", tk.END)
            if message:
                send_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M')
                # 显示在聊天界面
                self.textbox.insert(tk.END, f"{self.client.username} -> {recipient} ({send_time}):\n{message}\n")
                self.textbox.see(tk.END)
                # 向服务器发送消息
                self.client.send_message(f"PRIVATE|{self.client.username}|{recipient}|{send_time}|{message}")
            else:
                messagebox.showerror("错误", "发送的消息不能为空")
        else:
            messagebox.showerror("错误", "未选择私聊对象")

    # 发送群聊消息
    def send_group_message(self):
        message = self.input.get("1.0", tk.END)
        if message:
            send_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M')
            # 显示在聊天界面
            self.textbox.insert(tk.END, f"{self.client.username} -> public ({send_time}):\n{message}\n")
            self.textbox.see(tk.END)
            # 向服务器发送消息
            self.client.send_message(f"GROUP|{self.client.username}|{send_time}|{message}")
        else:
            messagebox.showerror("错误", "发送的消息不能为空")

    # 发送文件
    def send_file(self):
        # 选择文件
        file_path = filedialog.askopenfilename()
        if file_path:
            file_name = os.path.basename(file_path)
            file_size = os.path.getsize(file_path)
            # 选择接收方
            recipient = simpledialog.askstring("文件传输", "请输入接收方")
            if recipient:
                self.client.send_message(f"FILE_HEADER|{self.client.username}|{recipient}|{file_name}|{file_size}")
                threading.Thread(target=self.send_file_thread, args=(recipient, file_path, file_size, )).start()

    def send_file_thread(self, recipient, file_path, file_size):
        # 等待应答
        while not self.file_state:
            continue
        # 接收方不存在
        if self.file_state == 1:
            messagebox.showerror("错误", "文件接收方不存在")
            self.file_state = 0
        # 对方拒绝接收
        elif self.file_state == 2:
            messagebox.showerror("错误", "对方拒绝接收文件")
            self.file_state = 0
        # 开始发送
        elif self.file_state == 3 or self.file_state == 4:
            file_name = os.path.basename(file_path)
            target = recipient
            if self.file_state == 4:
                recipient = "SERVER"    # 离线文件的接收方为服务器
            # UI界面
            file_window = tk.Toplevel(self.window)
            file_window.title("文件传输")
            file_window.protocol("WM_DELETE_WINDOW", self.file_cancel)
            tk.Label(file_window, text=f"{file_name}文件上传中", font=10).pack()
            # 控制信息
            message = f"FILE_CONTENT|{file_name}|{recipient}"
            with open(file_path, "rb") as f:
                # 定位到先前已发送的位置
                f.seek(self.file_sent_size)
                total_sent = self.file_sent_size
                # 发送文件数据
                while self.file_state != 0 and total_sent < file_size:
                    data = f.read(2**15)
                    total_sent += len(data)
                    self.client.send_message(message, data)
            file_window.destroy()
            self.file_sent_size = 0
            if self.file_state != 0:
                # 发送完成
                self.client.send_message(f"FILE_END|{self.client.username}|{recipient}|{file_name}|{file_size}|{target}")
                messagebox.showinfo("文件传输", f"{file_name}文件发送完成")
                self.file_state = 0
            else:
                # 发送取消
                self.client.send_message(f"FILE_CANCEL|{recipient}|{file_name}")

    # 取消文件发送
    def file_cancel(self):
        self.file_state = 0

    # 发送语音
    def send_voice(self):
        # 选择接收方
        recipient = self.user_list.get("anchor")
        if recipient:
            self.client.send_message(f"VOIC_HEADER|{self.client.username}|{recipient}")
            # 创建发送线程
            threading.Thread(target=self.send_voice_thread, args=(recipient, )).start()
        else:
            messagebox.showerror("错误", "未选择接听方")

    def send_voice_thread(self, recipient):
        # 等待应答
        while not self.voice_state:
            continue
        # 对方拒绝接听
        if self.voice_state == 1:
            messagebox.showerror("错误", "对方拒绝接听")
            self.voice_state = 0
        # 开始发送
        elif self.voice_state == 2:
            # UI界面
            voice_window = tk.Toplevel(self.window)
            voice_window.title("语音通话")
            tk.Label(voice_window, text=f"正在与{recipient}通话...", font=10).pack()
            voice_window.protocol("WM_DELETE_WINDOW", self.close_voice)
            # 控制信息
            message = f"VOIC_CONTENT|{self.client.username}|{recipient}"
            # 发送语音数据
            while self.voice_state == 2:
                data = self.recording_stream.read(1024)
                self.client.send_message(message, data)
            # 语音通话结束
            voice_window.destroy()
            self.client.send_message(f"VOIC_END|{self.client.username}|{recipient}")

    # 结束语音通话
    def close_voice(self):
        self.voice_state = 0

    # 更新在线用户列表
    def update_online_users(self, message):
        user = message[4:]
        # 用户上线
        if message.startswith("ADD"):
            self.user_list.insert(tk.END, user)
        # 用户离线
        elif message.startswith("DEL"):
            for i in range(self.user_list.size()):
                if self.user_list.get(i) == user:
                    self.user_list.delete(i)
                    break

    # 接收私聊消息
    def recv_private_message(self, message):
        parts = message.split("|", maxsplit=3)
        sender = parts[0]
        recipient = parts[1]
        send_time = parts[2]
        msg = parts[3]
        if recipient == self.client.username:
            self.textbox.insert(tk.END, f"{sender} -> {recipient} ({send_time}):\n{msg}\n")
            self.textbox.see(tk.END)

    # 接收群聊消息
    def recv_group_message(self, message):
        parts = message.split("|", maxsplit=2)
        sender = parts[0]
        send_time = parts[1]
        msg = parts[2]
        self.textbox.insert(tk.END, f"{sender} -> public ({send_time}):\n{msg}\n")
        self.textbox.see(tk.END)

    # 接收文件
    def recv_file(self, message, data):
        # 询问是否接收在线文件
        if message.startswith("HEADER"):
            parts = message.split("|", maxsplit=3)
            sender = parts[1]
            file_name = parts[2]
            file_size = int(parts[3])
            threading.Thread(target=self.recv_file_thread, args=(sender, file_name, file_size, 1)).start()
        # 询问是否接收离线文件
        elif message.startswith("OFFLINE_HEADER"):
            parts = message.split("|", maxsplit=3)
            sender = parts[1]
            file_name = parts[2]
            file_size = int(parts[3])
            threading.Thread(target=self.recv_file_thread, args=(sender, file_name, file_size, 2)).start()
        # 接收文件
        elif message.startswith("CONTENT"):
            parts = message.split("|", maxsplit=1)
            file_name = parts[1]
            with open(file_name+".tmp", "ab") as f:
                f.write(data)
        # 接收完成
        elif message.startswith("END"):
            parts = message.split("|", maxsplit=1)
            file_name = parts[1]
            os.rename(file_name + ".tmp", file_name)
            threading.Thread(target=self.recv_file_thread, args=("", file_name, "", 3)).start()
        # 传输中断
        elif message.startswith("CANCEL"):
            parts = message.split("|", maxsplit=1)
            file_name = parts[1]
            threading.Thread(target=self.recv_file_thread, args=("", file_name, "", 4)).start()
        # 接收方不存在
        elif message.startswith("USER_NO_EXIST"):
            self.file_state = 1
        # 对方拒绝接收
        elif message.startswith("REJECT"):
            self.file_state = 2
        # 通知发送线程开始发送在线文件
        elif message.startswith("ACCEPT"):
            self.file_sent_size = int(message[7:])
            self.file_state = 3
        # 通知发送线程开始发送离线文件
        elif message.startswith("OFFLINE_USER"):
            self.file_sent_size = int(message[13:])
            self.file_state = 4

    def recv_file_thread(self, sender, file_name, file_size, mode):
        # 询问是否接收在线文件
        if mode == 1:
            if messagebox.askyesno("文件传输", f"{sender}给你发送了一个文件：\n{file_name}({file_size}Bytes)\n是否接收？"):
                file_sent_size = 0
                # 获取先前已接收的文件大小-断点续传
                if os.path.isfile(file_name+".tmp"):
                    file_sent_size = os.path.getsize(file_name+".tmp")
                self.client.send_message(f"FILE_ACCEPT|{self.client.username}|{sender}|{file_sent_size}")
            else:
                self.client.send_message(f"FILE_REJECT|{self.client.username}|{sender}")
        # 询问是否接收离线文件
        elif mode == 2:
            if messagebox.askyesno("文件传输", f"{sender}给你发送了一个离线文件：\n{file_name}({file_size}Bytes)\n是否接收？"):
                file_sent_size = 0
                # 获取先前已接收的文件大小-断点续传
                if os.path.isfile(file_name+".tmp"):
                    file_sent_size = os.path.getsize(file_name+".tmp")
                self.client.send_message(f"FILE_ACCEPT|{self.client.username}|SERVER|{file_sent_size}")
            else:
                self.client.send_message(f"FILE_REJECT|{self.client.username}|SERVER")
        # 文件接收完成
        elif mode == 3:
            messagebox.showinfo("文件传输", f"{file_name}文件接收完成")
        # 文件传输中断
        elif mode == 4:
            messagebox.showinfo("文件传输", f"{file_name}文件传输中断")

    # 接收语音
    def recv_voice(self, message, data):
        # 询问是否进行语音通话
        if message.startswith("HEADER"):
            parts = message.split("|", maxsplit=1)
            sender = parts[1]
            threading.Thread(target=self.recv_voice_thread, args=(sender, 1)).start()
        # 接听语音
        elif message.startswith("CONTENT"):
            self.playing_stream.write(data)
        # 对方关闭语音通话
        elif message.startswith("END"):
            parts = message.split("|", maxsplit=1)
            sender = parts[1]
            self.voice_state = 0
            threading.Thread(target=self.recv_voice_thread, args=(sender, 2)).start()
        # 对方拒绝接听
        elif message.startswith("REJECT"):
            self.voice_state = 1
        # 通知发送线程开始发送语音
        elif message.startswith("ACCEPT"):
            self.voice_state = 2

    def recv_voice_thread(self, sender, mode):
        # 询问是否同意语音通话
        if mode == 1:
            if messagebox.askyesno("语音通话", f"{sender}邀请你进行语音通话\n是否接受？"):
                self.client.send_message(f"VOIC_ACCEPT|{self.client.username}|{sender}")
                # 开启语音发送线程
                threading.Thread(target=self.send_voice_thread, args=(sender,)).start()
                self.voice_state = 2
            else:
                self.client.send_message(f"VOIC_REJECT|{self.client.username}|{sender}")
        # 语音通话关闭
        elif mode == 2:
            messagebox.showinfo("语音通话", "语音通话已关闭")

    # NAT穿透（全锥形）
    def nat_request(self):
        # 选择连接方
        recipient = self.user_list.get("anchor")
        if recipient:
            self.client.send_message(f"NAT_REQUEST|{self.client.username}|{recipient}")
        else:
            messagebox.showerror("错误", "未选择连接方")

    def nat_handle(self, message):
        # 询问是否同意NAT穿透
        if message.startswith("REQUEST"):
            parts = message.split("|", maxsplit=1)
            sender = parts[1]
            threading.Thread(target=self.nat_thread, args=(sender, 1)).start()
        # 同意NAT穿透
        elif message.startswith("ACCEPT"):
            parts = message.split("|", maxsplit=1)
            sender = parts[1]
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            # 向服务器发送消息以通知对方自己的公网ip地址与端口
            sock.sendto(f"NAT_SERVER|{sender}".encode("utf-8"), ("8.130.66.246", 6666))
            # 开启P2P通信线程
            threading.Thread(target=self.nat_send, args=(sock, sender)).start()
            threading.Thread(target=self.nat_recv, args=(sock,)).start()
        # 拒绝NAT穿透
        elif message.startswith("REJECT"):
            threading.Thread(target=self.nat_thread, args=("", 2)).start()
        # 获取对方公网ip地址与端口
        elif message.startswith("ADDRESS"):
            parts = message.split("|", maxsplit=1)
            self.MyPeer = eval(parts[1])

    def nat_thread(self, sender, mode):
        # 询问是否同意NAT穿透
        if mode == 1:
            if messagebox.askyesno("NAT穿透", f"{sender}请求建立连接\n是否接受？"):
                self.client.send_message(f"NAT_ACCEPT|{self.client.username}|{sender}")
                sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                time.sleep(1)
                # 向服务器发送消息以通知对方自己的公网ip地址与端口
                sock.sendto(f"NAT_SERVER|{sender}".encode("utf-8"), ("8.130.66.246", 6666))
                # 开启P2P通信线程
                threading.Thread(target=self.nat_send, args=(sock, sender)).start()
                threading.Thread(target=self.nat_recv, args=(sock,)).start()
            else:
                self.client.send_message(f"NAT_REJECT|{self.client.username}|{sender}")
        # 对方拒绝NAT穿透
        elif mode == 2:
            messagebox.showerror("错误", "对方拒绝建立连接")

    def nat_send(self, sock, recipient):
        # 等待服务器通知对方的公网ip地址与端口
        while not self.MyPeer:
            continue
        print(f"对方公网ip地址与端口号：{self.MyPeer}")
        # 尝试UDP打洞
        for i in range(5):
            sock.sendto("NAT_HELLO".encode("utf-8"), self.MyPeer)
            time.sleep(1)
        self.P2P = False
        # 发送P2P私聊消息
        while True:
            if self.P2P:
                message = self.input.get("1.0", tk.END)
                if message:
                    send_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M')
                    # 显示在聊天界面
                    self.textbox.insert(tk.END, f"{self.client.username} -> {recipient} ({send_time}):\n{message}\n")
                    self.textbox.see(tk.END)
                    try:
                        # 发送消息
                        sock.sendto(f"{self.client.username}|{recipient}|{send_time}|{message}".encode("utf-8"), self.MyPeer)
                    except socket.error as e:
                        # 连接出错
                        print(e)
                        sock.close()
                        self.MyPeer = None
                        self.P2P = False
                        raise SystemExit
                else:
                    messagebox.showerror("错误", "发送的消息不能为空")
                self.P2P = False

    def nat_recv(self, sock):
        while True:
            try:
                # 接收消息
                message = sock.recvfrom(1024)[0].decode("utf-8")
            except socket.error as e:
                # 连接出错
                print(e)
                sock.close()
                self.MyPeer = None
                self.P2P = False
                raise SystemExit
            else:
                if message.startswith("NAT_HELLO"):
                    # UDP打洞成功
                    print("connect!")
                    continue
                # 接收P2P私聊消息
                parts = message.split("|", maxsplit=3)
                sender = parts[0]
                recipient = parts[1]
                send_time = parts[2]
                msg = parts[3]
                if recipient == self.client.username:
                    self.textbox.insert(tk.END, f"(P2P){sender} -> {recipient} ({send_time}):\n{msg}\n")
                    self.textbox.see(tk.END)

    def P2P_chat(self):
        self.P2P = True

    # 接收来自服务器的消息
    def receive_messages(self):
        while True:
            message, data = self.client.receive_message()
            if message.startswith("UPDATE_USERS"):
                self.update_online_users(message[13:])
            elif message.startswith("PRIVATE"):
                self.recv_private_message(message[8:])
            elif message.startswith("GROUP"):
                self.recv_group_message(message[6:])
            elif message.startswith("FILE"):
                self.recv_file(message[5:], data)
            elif message.startswith("VOIC"):
                self.recv_voice(message[5:], data)
            elif message.startswith("NAT"):
                self.nat_handle(message[4:])

#############################################################################


if __name__ == '__main__':
    Server_ip = "8.130.66.246"     # 服务器公网ip地址
    Server_port = 8888

    # 创建聊天室客户端
    client = Client(Server_ip, Server_port)
    # 打开登录窗口
    LoginWindow(client)
    # 打开聊天窗口
    ChatWindow(client)
