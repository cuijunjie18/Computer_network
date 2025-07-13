#!/bin/bash

cd bin
SERVER_IP=smtp.qq.com
SERVER_PORT=25 # SMTP服务器的通用port

./SMTP_client $SERVER_IP $SERVER_PORT