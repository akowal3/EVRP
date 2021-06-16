import sys

from flask import Flask, request, jsonify
from flask_cors import CORS

from backend import EvrpBackend
from routing_request import RoutingRequest

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")

app = Flask(__name__)
CORS(app)

backend = EvrpBackend().from_pickle()

print(f'Node count: {len(backend.chargers)}')
print(f'Edge count: {len(backend.edges)}')


@app.route('/evroute/', methods=['POST', 'OPTIONS'])
def evroute():
    # try:
    #     req = RoutingRequest(request.json)
    # except Exception as e:
    #     print(e)
    #     return backend.construct_error(e)

    response = {}
    if request.method == 'POST':
        req = RoutingRequest(request.json)
        response = backend.process_request(req)

    response = jsonify(response)
    return response

# if __name__ == '__main__':
#     backend = EvrpBackend.from_pickle()
#
#     res = backend.osrm.route_dt([(50.2817784357959, 19.56803391490621), (50.26236162833732, 19.043632959275783)])
#
#     c = Car(0.2)
#
#     while True:
#         src = int(input(f"Give source (0 - {len(backend.chargers) - 1}): "))
#         dst = int(input(f"Give destination (0 - {len(backend.chargers) - 1}): "))
#         result = backend.route(src, dst, c)
#
#         print(result)
