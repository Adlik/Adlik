# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a script for training mnist model.
"""
import os

import keras
import numpy as np


def process_dataset():
    # Import the data
    (x_train, y_train), (x_test, y_test) = keras.datasets.mnist.load_data()

    x_train, x_test = x_train / 255.0, x_test / 255.0

    # Reshape the data
    x_train = np.reshape(x_train, (60000, 28, 28, 1))
    x_test = np.reshape(x_test, (10000, 28, 28, 1))
    return x_train, y_train, x_test, y_test


def create_model():
    model = keras.models.Sequential()
    model.add(keras.layers.Conv2D(32, kernel_size=(3, 3),
                                  activation='relu',
                                  input_shape=(28, 28, 1)))
    model.add(keras.layers.Conv2D(64, (3, 3), activation='relu'))
    model.add(keras.layers.MaxPooling2D(pool_size=(2, 2)))
    model.add(keras.layers.Dropout(0.25))
    model.add(keras.layers.Reshape((9216,)))
    model.add(keras.layers.Dense(128, activation='relu'))
    model.add(keras.layers.Dropout(0.5))
    model.add(keras.layers.Dense(10, activation='softmax'))
    model.compile(optimizer='adam', loss='sparse_categorical_crossentropy', metrics=['accuracy'])
    return model


def _train():
    x_train, y_train, x_test, y_test = process_dataset()
    model = create_model()
    model.fit(x_train, y_train, epochs=2, verbose=1)
    model.evaluate(x_test, y_test)
    save_path = os.path.join(os.path.dirname(__file__), 'model', 'mnist.h5')
    dir_name = os.path.dirname(save_path)
    os.makedirs(dir_name, exist_ok=True)
    model.save(save_path)


if __name__ == '__main__':
    _train()
