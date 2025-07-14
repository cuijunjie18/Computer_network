from socket import *
import base64

# 发件人邮箱账号
sender = '287852@qq.com'
# 发件人邮箱授权码（注意：实际使用时需替换为真实授权码）
password = 'exmogdcjd' 
# 收件人邮箱账号
receiver = '2293366@qq.com'

breakpoint()

# QQ邮箱服务器配置
mailserver = ('smtp.qq.com', 25)  # 使用587端口（STARTTLS）

# 创建套接字并建立TCP连接
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect(mailserver)

# 接收服务器初始响应
recv = clientSocket.recv(1024).decode()
print(recv)
if recv[:3] != '220':
    print('未从服务器收到220回复。')

# 发送EHLO命令（QQ邮箱要求使用EHLO）
ehloCommand = 'EHLO Alice\r\n'
clientSocket.send(ehloCommand.encode())
recv1 = clientSocket.recv(1024).decode()
print(recv1)
if recv1[:3] != '250':
    print('未从服务器收到250回复。')

# 发送STARTTLS命令（开始加密连接）
starttlsCommand = 'STARTTLS\r\n'
clientSocket.send(starttlsCommand.encode())
recv_tls = clientSocket.recv(1024).decode()
print(recv_tls)
if recv_tls[:3] != '220':
    print('未从服务器收到220回复（STARTTLS）。')

# 注意：实际应用中这里需要升级到SSL连接
# 但标准socket库不直接支持TLS，通常使用ssl.wrap_socket()
# 为简化演示，我们继续在未加密连接发送（实际不可行）
# 实际使用时需添加以下代码：
import ssl
clientSocket = ssl.wrap_socket(clientSocket)

# 认证登录 - 发送AUTH LOGIN命令
authCommand = 'AUTH LOGIN\r\n'
clientSocket.send(authCommand.encode())
recv_auth = clientSocket.recv(1024).decode()
print(recv_auth)

# 发送Base64编码的用户名
username = base64.b64encode(sender.encode()).decode() + '\r\n'
clientSocket.send(username.encode())
recv_user = clientSocket.recv(1024).decode()
print(recv_user)

# 发送Base64编码的密码
password_b64 = base64.b64encode(password.encode()).decode() + '\r\n'
clientSocket.send(password_b64.encode())
recv_pass = clientSocket.recv(1024).decode()
print(recv_pass)

# 发送MAIL FROM命令
mailFromCommand = f'MAIL FROM: <{sender}>\r\n'
clientSocket.send(mailFromCommand.encode())
recv2 = clientSocket.recv(1024).decode()
print(recv2)
if recv2[:3] != '250':
    print('MAIL FROM命令未收到250回复。')

# 发送RCPT TO命令
rcptToCommand = f'RCPT TO: <{receiver}>\r\n'
clientSocket.send(rcptToCommand.encode())
recv3 = clientSocket.recv(1024).decode()
print(recv3)
if recv3[:3] != '250':
    print('RCPT TO命令未收到250回复。')

# 发送DATA命令
dataCommand = 'DATA\r\n'
clientSocket.send(dataCommand.encode())
recv4 = clientSocket.recv(1024).decode()
print(recv4)
if recv4[:3] != '354':
    print('未收到354回复（开始邮件输入）。')

# 构造邮件内容
msg = f'''From: "Python测试" <{sender}>
To: "收件人姓名" <{receiver}>
Subject: Python测试邮件
Content-Type: text/plain; charset="utf-8"

这是一封来自Python的测试邮件
'''

endmsg = "\r\n.\r\n"

# 发送邮件数据
clientSocket.send(msg.encode())
# 发送结束标记
clientSocket.send(endmsg.encode())
recv5 = clientSocket.recv(1024).decode()
print(recv5)
if recv5[:3] != '250':
    print('邮件发送未收到250回复。')

# 发送QUIT命令
quitCommand = 'QUIT\r\n'
clientSocket.send(quitCommand.encode())
recv6 = clientSocket.recv(1024).decode()
print(recv6)

# 关闭连接
clientSocket.close()
print("邮件发送流程完成")