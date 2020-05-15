from typing import Callable, Optional, TypeVar

_Type1 = TypeVar('_Type1')
_Type2 = TypeVar('_Type2')


def map_optional(value: Optional[_Type1], func: Callable[[_Type1], _Type2]) -> Optional[_Type2]:
    if value is None:
        return None

    return func(value)
