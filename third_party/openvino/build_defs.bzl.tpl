# Build configurations for OpenVINO inference engine.

def if_openvino(if_true, if_false=[]):
  """Tests whether Openvino was enabled during the configure process."""
  if %{openvino_is_configured}:
    return if_true
  return if_false

