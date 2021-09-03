# Adlik serving support interfaces

## grpc interfaces

    1. Client SDK(c++)
        
      predict_service.proto
      #Function prototype
      **rpc predict(PredictRequest) returns (PredictResponse)**

        #Function Description
        Perform deep learning model inference.

        #Parameter Description
        - PredictRequest ： const pointer
          A constant pointer, defined by protobuf, for inference requests.

        - returns : PredictResponse : pointer
          The inference result message returned by the server, defined by portobuf.

      #Function prototype
      **rpc getModelMeta(GetModelMetaRequest) returns (GetModelMetaResponse)**

        #Function Description
        Obtain the configuration information of the deep learning model.

        #Parameter Description
        - GetModelMetaRequest ： const pointer
          A constant pointer, request for model configuration information, defined by protobuf.

        - returns : GetModelMetaResponse: pointer
          The model configuration message is returned by the server, defined by protobuf.

      model_operate_service.proto
      #Function prototype
      **rpc addModel(ModelOperateRequest) returns (ModelOperateResponse)**
      **rpc addModelVersion(ModelOperateRequest) returns (ModelOperateResponse)**
      **rpc deleteModel(ModelOperateRequest) returns (ModelOperateResponse)**
      **rpc deleteModelVersion(ModelOperateRequest) returns (ModelOperateResponse)**
      **rpc activateModel(ModelOperateRequest) returns (ModelOperateResponse)**
      **rpc queryModel(ModelOperateRequest) returns (ModelOperateResponse)**

        #Function Description
        Manage models and tasks, including model and model version addition, deletion, query and activation.

        #Parameter Description
        - ModelOperateRequest ： const pointer
          Model management request, defined by protobuf.

        - returns : ModelOperateResponse : pointer
          The model management operation response information is returned by the server, defined by portobuf.

      For c++ client sdk usage examples, please refer to [cpp_client](cpp/README.md).
    
    2. Client SDK(python)

      #Function prototype
      ** class PredictContext(model_name, model_version, signature, url, protocol, grpc_domain, credentials, verbose)**

        #Function Description
        An PredictContext object is used to run predict on server for a specific model. Once created an InferContext 
        object can be used repeatedly to perform inference using the model.

        #Parameter Description
        - model_name : str
          The name of the model to get status or do prediction.
        - model_version : int
          The version of the model to use for inference.Default is None which indicate that the latest (i.e. highest 
          version number) version should be used.
        - signature : str
          The signature of the model.
        - url : str
          The inference server URL, e.g. localhost:8500.
        - protocol : ProtocolType
          The protocol used to communicate with the server, like grpc or http.
        - credentials : str
          Path of credentials, required when grpc SSL-enabled. Default is None.
        - verbose : bool
          If True generate verbose output.

        #Function
        **model_config()**

        #Function Description
        Get model meta information, should send get meta request to server and parse meta from response.

        #Function
        **run(inputs, outputs, batch_size, iteration)**

        #Function Description
        Run predict using the supplied 'inputs' to calculate the outputs specified by 'outputs'.

        #Parameter Description
        - inputs: dict
          Dictionary from input name to the value(s) for that input. An input value is specified as  a numpy
          array. Each input in the dictionary maps to a list of values (i.e. a list of numpy array objects), where the
          length of the list must equal the 'batch_size'.
        - outputs: list
          list of string or dictionary of name: type indicating the output names should be returned.
        - batch_size: int
          The batch size of the prediction. Each input must provide an appropriately sized batch of inputs.
        - iteration: int
          iteration times for send request, just for evaluating time of server process request.
        - return: dict
          A dictionary from output name to the list of values for that output (one list element for each entry of the batch).

        #Function
        **async_run(inputs, outputs, batch_size, iteration)**

        #Function Description
        Run asynchronously predict using the supplied 'inputs' to calculate the outputs specified by 'outputs'.

        #Function
        **process_response(output_names, response)**

        #Function Description
        Processing predicted response and convert the tensor type.

        #Parameter Description
        - output_names: list
          The model outputs the name of the op.
        - response: dict
          Output of prediction results in tensor proto format.

      For python client sdk usage examples, please refer to [image_client.py](python/image_client.py).

## http interfaces

    Client SDK(python)

      #Function prototype
      ** class PredictContext(model_name, model_version, signature, url, protocol, grpc_domain, credentials, verbose)**

        #Function Description
        An PredictContext object is used to run predict on server for a specific model. Once created an InferContext 
        object can be used repeatedly to perform inference using the model.

        #Parameter Description
        - model_name : str
          The name of the model to get status or do prediction.
        - model_version : int
          The version of the model to use for inference.Default is None which indicate that the latest (i.e. highest 
          version number) version should be used.
        - signature : str
          The signature of the model.
        - url : str
          The inference server URL, e.g. localhost:8500.
        - protocol : ProtocolType
          The protocol used to communicate with the server, like grpc or http.
        - credentials : str
          Path of credentials, required when grpc SSL-enabled. Default is None.
        - verbose : bool
          If True generate verbose output.

        #Function
        **model_config()**

        #Function Description
        Get model meta information, should send get meta request to server and parse meta from response.

        #Function
        **run(inputs, outputs, batch_size, iteration)**

        #Function Description
        Run predict using the supplied 'inputs' to calculate the outputs specified by 'outputs'.

        #Parameter Description
        - inputs: dict
          Dictionary from input name to the value(s) for that input. An input value is specified as  a numpy
          array. Each input in the dictionary maps to a list of values (i.e. a list of numpy array objects), where the
          length of the list must equal the 'batch_size'.
        - outputs: list
          list of string or dictionary of name: type indicating the output names should be returned.
        - batch_size: int
          The batch size of the prediction. Each input must provide an appropriately sized batch of inputs.
        - iteration: int
          iteration times for send request, just for evaluating time of server process request.
        - return: dict
          A dictionary from output name to the list of values for that output (one list element for each entry of the batch).

        #Function
        **async_run(inputs, outputs, batch_size, iteration)**

        #Function Description
        Run asynchronously predict using the supplied 'inputs' to calculate the outputs specified by 'outputs'.

        #Function
        **process_response(output_names, response)**

        #Function Description
        Processing predicted response and convert the tensor type.

        #Parameter Description
        - output_names: list
          The model outputs the name of the op.
        - response: dict
          Output of prediction results in tensor proto format.

      Currently, for the http interface, only python client sdk is provided. For python client sdk usage examples,
      please refer to [image_client.py](python/image_client.py).
