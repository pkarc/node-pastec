# node-pastec


## Contribute

To work on the code, first fetch the bundled pastec submodule, then build node-pastec and run the tests.

```
git submodule update --init
npm install
npm test
```

## Dependencies

In order to compile pastec submodule, you need OpenCV. On Ubuntu 14.10, this package can be installed using the following command:

```
sudo apt-get install libopencv-dev
```