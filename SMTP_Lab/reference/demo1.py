import smtplib
from email.mime.text import MIMEText
from email.utils import formataddr

# 发件人邮箱账号
sender = '2871843852@qq.com'
# 发件人邮箱密码或授权码
password = 'exmojhygmmjgdcjd'
# 收件人邮箱账号
receiver = '2293393766@qq.com'

# 创建邮件消息对象
message = MIMEText('这是一封来自Python的测试邮件', 'plain', 'utf-8')
# 设置邮件消息对象的发件人和收件人
message['From'] = formataddr(('Python测试', sender))
message['To'] = formataddr(('收件人姓名', receiver))
# 设置邮件消息对象的主题
message['Subject'] = 'Python测试邮件'

try:
    # 连接SMTP服务器
    server = smtplib.SMTP_SSL('smtp.qq.com', 465)
    # 登录邮箱账号
    server.login(sender, password)
    # 发送邮件
    server.sendmail(sender, [receiver], message.as_string())
    # 关闭SMTP连接
    server.quit()
    print("邮件发送成功")
except Exception as e:
    print("邮件发送失败:", e)
