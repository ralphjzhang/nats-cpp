import cppyy
import time
import subprocess
import signal
from pytest import fixture

cppyy.add_include_path("inc")
cppyy.add_include_path("inc/asio")
cppyy.add_library_path("build")
cppyy.load_library("nats-st")
cppyy.include("st/nats.hpp")

nats = cppyy.gbl.nats
std = cppyy.gbl.std


@fixture
def nats_server():
    server = subprocess.Popen(["./st/nats-server"])
    yield server
    server.send_signal(signal.SIGINT)


@fixture
def nats_client():
    conn = nats.run("4222")
    thread = nats.run(conn, "4222")
    yield conn
    thread.join()


def test_me(nats_client):
    nats_client.subscribe("subj", "", cppyy.gbl.create_callback())

    assert True


cppyy.cppdef("""\
auto print(std::function<int(int, int, std::string&&)> cb, int a, int b, std::string c) {
    std::cout << "hello, " << cb(a, b, std::move(c)) << std::endl;
}
""")


def print_me(subj, queue, data):
    print('kasdffasfasdf')
    # print(subj, queue, data)


if __name__ == '__main__':
    t = std.thread()
    conn = nats.run(t, "4222")
    nats.subscribe(conn, "subj", print_me)
    time.sleep(2)
    t.join()

    # conn = nats.TcpConnection()
    # thread = nats.run(conn, "4222")
    # conn.subscribe("subj", "", print_me)
    # thread.join()

    # cppyy.gbl.print(print_me, 42, 2, "")
