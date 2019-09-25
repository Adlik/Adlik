# -*- coding: utf-8 -*-
# pylint: disable-all

import keras
from keras.layers import Conv2D, Dense, Flatten, MaxPooling2D
from keras.models import Sequential


def get_model():
    model = Sequential()
    model.add(Conv2D(32, kernel_size=(3, 3),
                     activation='relu',
                     input_shape=(28, 28, 1)))
    model.add(Conv2D(64, (3, 3), activation='relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Flatten())
    model.add(Dense(128, activation='relu'))
    model.add(Dense(10, activation='softmax'))

    return None, model


def load_data():
    img_rows, img_cols = 28, 28
    (x_train, y_train), (x_test, y_test) = keras.datasets.mnist.load_data()
    x_train = x_train.reshape(x_train.shape[0], img_rows, img_cols, 1)
    x_test = x_test.reshape(x_test.shape[0], img_rows, img_cols, 1)
    x_train = x_train.astype('float32')
    x_test = x_test.astype('float32')
    x_train /= 255
    x_test /= 255
    return (x_train, y_train), (x_test, y_test)


def main(h5_file):
    _, model = get_model()
    (x_train, y_train), (x_test, y_test) = load_data()
    model.compile(optimizer='adam', loss='sparse_categorical_crossentropy', metrics=['accuracy'])
    model.fit(x_train, y_train, batch_size=256, epochs=5, verbose=1)
    model.evaluate(x_test, y_test)
    model.save(h5_file)


if __name__ == "__main__":
    main('model/model.h5')
