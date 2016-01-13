all:
	node-gyp configure build

clean:
	node-gyp clean

temp:
	rm -rf tmp/node-pastec
	mkdir -p tmp/node-pastec
	cp -r README* COPYING *.js* binding.gyp src deps test tmp/node-pastec
	cd tmp/node-pastec && rm -rf deps/*/.git* deps/*/*.o deps/*/*.a

package: temp
	cd tmp && tar -czvf node-pastec.tgz node-pastec

check:
	//npm test