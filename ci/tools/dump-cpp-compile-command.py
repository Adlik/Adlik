import json
import sys

import ci.extra_actions_base_pb2


def _read_cpp_compile_info():
    action_info = ci.extra_actions_base_pb2.ExtraActionInfo()
    action_info.ParseFromString(sys.stdin.buffer.read())

    return action_info.Extensions[ci.extra_actions_base_pb2.CppCompileInfo.cpp_compile_info]


def main():
    cpp_compile_info = _read_cpp_compile_info()

    item = {
        'file': cpp_compile_info.source_file,
        'arguments': [cpp_compile_info.tool, *cpp_compile_info.compiler_option],
        'output': cpp_compile_info.output_file
    }

    json.dump(item, sys.stdout)


if __name__ == "__main__":
    main()
