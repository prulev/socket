import socket
import struct

# 创建一个 TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 绑定 IP 地址和端口号
server_socket.bind(('10.89.16.238', 8886))

print(1)

# 监听连接
server_socket.listen(1)

print(2)

# 接受客户端连接
client_socket, client_address = server_socket.accept()
print("New connection from {client_address}")

print(3)

data_list = [];
while True:
    # 接收客户端发送的数据
    data = client_socket.recv(64)

    if not data:
        # 如果客户端关闭了连接，就退出循环
        print("Connection from {client_address} closed")
        break

    # 解码数据
    pos_X, pos_Y = map(float, data.split())
    # 储存数据
    data_list.append((pos_X, pos_Y))
    if len(data_list) > 5:
        data_list.pop(0)

    X = sum([x[0] for x in data_list]) / len(data_list)
    Y = sum([x[1] for x in data_list]) / len(data_list)

    # 将处理后的数据返回给客户端
    # send_data = str(X) + ' ' +  str(Y) + '\n'
    # print(send_data)
    # client_socket.send(send_data.encode())

    print(data_list)

# 关闭 socket
client_socket.close()
server_socket.close()
