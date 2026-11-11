import socket, subprocess, time

server = subprocess.Popen(['./servidor'], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
try:
    for _ in range(50):
        try:
            a = socket.create_connection(('127.0.0.1', 8080), timeout=1)
            break
        except OSError:
            time.sleep(.05)
    else:
        raise AssertionError('servidor não iniciou')
    a.settimeout(2)
    b = socket.create_connection(('127.0.0.1', 8080), timeout=2)
    b.settimeout(2)
    def receive(s):
        def read(n):
            data = b''
            while len(data) < n:
                part = s.recv(n - len(data))
                assert part
                data += part
            return data
        h = read(8)
        return h[:3], read(int(h[4:7]))
    a.sendall(b'REQ|0')
    a.sendall(b'00|VEL|002|75')
    assert receive(a)[0] == b'RES'
    assert receive(a) == (b'ACK', b'')
    b.sendall(b'VEL|002|xx')
    assert receive(b)[0] == b'ERR'
    b.sendall(b'ABC|000|')
    assert receive(b)[0] == b'ERR'
    b.sendall(b'REQ|001|x')
    assert receive(b)[0] == b'ERR'
    a.close(); b.close()
    print('Testes PTR passaram')
finally:
    server.terminate()
    server.wait(timeout=3)
