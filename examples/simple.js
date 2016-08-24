/**
 * Created by pkarc on 8/04/16.
 */
var nodePastec = new require("../");

var imageData = "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";

var pastec = new nodePastec.Pastec("backwardIndex.dat", "visualWordsORB.dat");
var imageTag = "cocaCola";

//Add image to index
console.log("####### Adding image from buffer whit id: 111 #######");
var buf = new Buffer(imageData, 'base64');
console.log(pastec.addImage(111, buf, imageTag));

//Search image from a buffer
console.log("####### Search Image from buffer #######");
var buf = new Buffer(imageData, 'base64');
console.log(pastec.searchImage(buf));

//Save index
console.log("####### Save index #######");
console.log(pastec.saveIndex('savedBackwardIndex.dat'));

//Clear index
console.log("####### Clearing index #######");
console.log(pastec.clearIndex());

//Load index
console.log("####### Save index #######");
console.log(pastec.loadIndex('savedBackwardIndex.dat'));

//Remove image from index
console.log("####### Removing Image whit id: 111 #######");
console.log(pastec.removeImage(111));
