from typing import Dict, List


class Repository:
    def __init__(self):
        self._source_models: List[type] = []
        self._target_models: Dict[str, type] = {}

    def register_source_model(self, model: type):
        self._source_models.append(model)

        return model

    def register_target_model(self, serving_type):
        def _register(model: type):
            self._target_models[serving_type] = model

            return model

        return _register

    def get_source_models(self):
        return self._source_models

    def get_target_model(self, serving_type):
        return self._target_models[serving_type]


REPOSITORY = Repository()
