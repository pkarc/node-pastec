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

## Usage

Simple example, included as examples/simple.js:

### Import module

```js
var nodePastec = new require("../");
```

### Initialize module

```js
var pastec = new nodePastec.Pastec("backwardIndex.dat", "visualWordsORB.dat");
```

### Add Image to index

```js
var imageData = ...//Buffer
var imageTag = "cocaCola";

console.log("####### Adding image from buffer with id: 111 #######");
var buf = new Buffer(imageData, 'base64');
console.log(pastec.addImage(111, buf, imageTag));
```

### Search image from a buffer

```js
console.log("####### Search Image from buffer #######");
var buf = new Buffer(imageData, 'base64');
console.log(pastec.searchImage(buf));
```

### Remove image from index

```js
console.log("####### Removing Image whit id: 111 #######");
console.log(pastec.removeImage(111));
```

### Add Images