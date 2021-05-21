import sys

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")
from bindings import Router, Car

from backend import EvrpBackend

if __name__ == '__main__':
    backend = EvrpBackend()
    backend.from_pickle()
    nodes, edges = backend.get_cpp()

    router = Router(nodes, edges)

    c = Car(0.3)

    while True:
        src = int(input(f"Give source (0 - {len(nodes)}): "))
        dst = int(input(f"Give destination (0 - {len(nodes)}): "))
        result = router.route(src, dst, c)
        t = router.route_internal(src, dst, c)
        print(result)
        print(t)
