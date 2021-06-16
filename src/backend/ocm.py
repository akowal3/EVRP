from typing import Dict, Any, List

import requests

from charger import Charger


class OCM:
    def __init__(
            self,
            host: str,
            api_key: str) -> None:
        self.host = host
        self.api_key = api_key

    @staticmethod
    def format_options(options: Dict[str, str]) -> str:
        """
        Formats dictionary of additional options to your OSRM request into a
        concatenated string format.
        """
        return "&".join([f"{k}={v if type(v) is not list else ','.join([str(val) for val in v])}" for k, v in options.items()])

    def make_request(self, options: Dict[str, Any]) -> List[Dict[str, Any]]:
        options['key'] = self.api_key
        options = self.format_options(options)
        url = f"{self.host}/v3/poi/?/{options}"
        response = requests.get(url)
        return response.json()

    def get_chargers_by_country(self, country_code: str, maxresults: int = 5) -> List[Charger]:
        res = self.make_request(
            options={'output': 'json', 'countrycode': country_code,
                     'maxresults': maxresults, 'compact': True, 'verbose': False,
                     'connectiontypeid': [33, 27], 'levelid': 3, 'minpowerkw': 50}
        )
        return [Charger.from_ocm(ocm_charger, i) for i, ocm_charger in enumerate(res)]
