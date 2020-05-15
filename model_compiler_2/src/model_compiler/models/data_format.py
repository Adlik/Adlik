import enum
from enum import Enum
from typing import Optional

from ..protos.generated.model_config_pb2 import ModelInput


class DataFormat(Enum):
    CHANNELS_FIRST = enum.auto()
    CHANNELS_LAST = enum.auto()


def as_model_config_data_format(data_format: Optional[DataFormat]):
    if data_format is None:
        result = ModelInput.FORMAT_NONE  # pylint: disable=no-member
    elif data_format == DataFormat.CHANNELS_FIRST:
        result = ModelInput.FORMAT_NCHW  # pylint: disable=no-member
    else:
        result = ModelInput.FORMAT_NHWC  # pylint: disable=no-member

    return result
