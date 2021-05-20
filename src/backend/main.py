import sys

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")
from bindings import Router, Car

from src.backend.backend import EvrpBackend

if __name__ == '__main__':
    backend = EvrpBackend()
    backend.from_pickle()
    nodes, edges = backend.get_cpp()

    router = Router(nodes, edges)

    c = Car(0.3)

    while True:
        src = input(f"Give source (0 - {len(nodes)}): ")
        dst = input(f"Give destination (0 - {len(nodes)}): ")
        result = router.route(int(src), int(dst), c)
        print(result)
