BOARD=arduino:avr:mini
SKETCH=~/Code/EvaCockpit/sketch
PORT=/dev/ttyUSB0

default: compile

compile:
	arduino-cli compile --fqbn ${BOARD} ${SKETCH}

install: compile
	arduino-cli upload -p ${PORT} --fqbn ${BOARD} ${SKETCH}
