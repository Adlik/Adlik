import paddle
import paddlehub as hub


def export_onnx_mobilenetv2(save_path):
    ''' export PaddlePaddle model to ONNX format
    Args:
        save_path(str): Path to save exported ONNX model
    Returns:
        None
    '''
    model = hub.Module(name="resnet50_vd_imagenet_ssld")
    input_spec = paddle.static.InputSpec(
            [1, 3, 224, 224], "float32", "image")
    paddle.onnx.export(model, save_path,
                       input_spec=[input_spec],
                       opset_version=10)


if __name__ == '__main__':
    save_path = './model/resnet50'
    model = export_onnx_mobilenetv2(save_path)
