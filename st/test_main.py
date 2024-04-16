import cppyy
import subprocess
import signal
from pytest import fixture

cppyy.add_include_path("src")
cppyy.add_include_path("inc")
cppyy.add_include_path("inc/asio")
cppyy.include("nats.hpp")

nats = cppyy.gbl.nats


@fixture
def nats_server():
    server = subprocess.Popen(["./st/nats-server"])
    yield server
    server.send_signal(signal.SIGINT)


def test_me(nats_server):
    conn = nats.TcpConnection()
    conn.run("8888")

    assert (True)
