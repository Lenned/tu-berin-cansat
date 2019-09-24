/*
 Spacecraft.js simulates a small spacecraft generating telemetry.
*/

var SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline')
 
var serialport = new SerialPort("/dev/tty.usbserial-DN02N1QJ", {
    baudRate: 9600,
});

const parser = serialport.pipe(new Readline({ delimiter: '\r\n' }));

var accXVal = 0.0;
var accYVal = 0.0;
var accZVal = 0.0;
var gyroXVal = 0.0;
var gyroYVal = 0.0;
var gyroZVal = 0.0;
var mpuTempVal = 0.0;
var tempVal = 0.0;
var pressureVal = 0.0;
var altitudeVal = 0.0;
var extTempVal = 0.0;

var accXSlope = 0.0;
var accYSlope = 0.0;
var accZSlope = 0.0;
var gyroXSlope = 0.0;
var gyroYSlope = 0.0;
var gyroZSlope = 0.0;
var pressureSlope = 0.0;
var altitudeSlope = 0.0;

var flightState = 0;
var liveTime = 0;
var missionTime = 0;

var oldTime = 0;
var diffTime = 0;

var motorOn = 0;
var parachuteOut = 0;
var autoChange = 0;
var isDriving = 0;
var didDrive = 0;
var drivingTime = 0;

function Spacecraft() {
    this.state = {
        "accX": 0.0,
        "accY": 0.0,
        "accZ": 0.0,
        "gyroX": 0.0,
        "gyroY": 0.0,
        "gyroZ": 0.0,
        "mpuTemp": 0.0,
        "temp": 0.0,
        "pressure": 0.0,
        "altitude": 0.0,
        "extTemp": 0.0,
        "dataRate": 0.0,
        "accXSlope": 0.0,
        "accYSlope": 0.0,
        "accZSlope": 0.0,
        "gyroXSlope": 0.0,
        "gyroYSlope": 0.0,
        "gyroZSlope": 0.0,
        "pressureSlope": 0.0,
        "altitudeSlope": 0.0,
        "flightState": 0,
        "liveTime": 0,
        "missionTime": 0,
        "motorOn": 0,
        "parachuteOut": 0,
        "autoChange": 0,
        "isDriving": 0,
        "didDrive": 0,
        "drivingTime": 0
    };
    this.history = {};
    this.listeners = [];
    Object.keys(this.state).forEach(function (k) {
        this.history[k] = [];
    }, this);

    setInterval(function () {
        this.updateState();
        this.generateTelemetry();
    }.bind(this), 50);
};

Spacecraft.prototype.uplink = function (data) {
    serialport.write(data);

    console.log("Command sent.");
}

parser.on('data', function (frame) {
    console.log(">>", frame);
    if(typeof frame !== "undefined") {
        var csvString = frame.toString();
        var splitArr = csvString.split(",");
        if(splitArr[0] == 'A') {
            accXVal = parseFloat(splitArr[1]);
            accYVal = parseFloat(splitArr[2]);
            accZVal = parseFloat(splitArr[3]);
            gyroXVal = parseFloat(splitArr[4]);
            gyroYVal = parseFloat(splitArr[5]);
            gyroZVal = parseFloat(splitArr[6]);
            mpuTempVal = parseFloat(splitArr[7]);
            tempVal = parseFloat(splitArr[8]);
            pressureVal = parseFloat(splitArr[9]);
            altitudeVal = parseFloat(splitArr[10]);
            extTempVal = parseFloat(splitArr[11]);

            flightState = parseInt(splitArr[12]);
            liveTime = parseInt(splitArr[13]);
            missionTime = parseInt(splitArr[14]);

            let newTime = Date.now();
            diffTime = newTime - oldTime;
            oldTime = newTime;
        } else if(splitArr[0] == 'B') {
            motorOn = parseInt(splitArr[1]);
            parachuteOut = parseInt(splitArr[2]);
            autoChange = parseInt(splitArr[3]);
            isDriving = parseInt(splitArr[4]);
            didDrive = parseInt(splitArr[5]);
            drivingTime = parseInt(splitArr[6]);
        }
    }
});

Spacecraft.prototype.updateState = function () {
    this.state["accX"] = accXVal;
    this.state["accY"] = accYVal;
    this.state["accZ"] = accZVal;
    this.state["gyroX"] = gyroXVal;
    this.state["gyroY"] = gyroYVal;
    this.state["gyroZ"] = gyroZVal;
    this.state["mpuTemp"] = mpuTempVal;
    this.state["temp"] = tempVal;
    this.state["pressure"] = pressureVal;
    this.state["altitude"] = altitudeVal;
    this.state["extTemp"] = altitudeVal;
    this.state["dataRate"] = (1000 / diffTime) * 2;

    this.state["accXSlope"] = accXSlope;
    this.state["accYSlope"] = accYSlope;
    this.state["accZSlope"] = accZSlope;
    this.state["gyroXSlope"] = gyroXSlope;
    this.state["gyroYSlope"] = gyroYSlope;
    this.state["gyroZSlope"] = gyroZSlope;
    this.state["pressureSlope"] = pressureSlope;
    this.state["altitudeSlope"] = altitudeSlope;

    this.state["flightState"] = flightState;
    this.state["liveTime"] = liveTime;
    this.state["missionTime"] = missionTime;
    this.state["motorOn"] = motorOn;
    this.state["parachuteOut"] = parachuteOut;
    this.state["autoChange"] = autoChange;
    this.state["isDriving"] = isDriving;
    this.state["didDrive"] = didDrive;
    this.state["drivingTime"] = drivingTime;
};

/**
 * Takes a measurement of spacecraft state, stores in history, and notifies 
 * listeners.
 */
Spacecraft.prototype.generateTelemetry = function () {
    var timestamp = Date.now(), sent = 0;
    Object.keys(this.state).forEach(function (id) {
        var state = { timestamp: timestamp, value: this.state[id], id: id};
        this.notify(state);
        this.history[id].push(state);
    }, this);
};

Spacecraft.prototype.notify = function (point) {
    this.listeners.forEach(function (l) {
        l(point);
    });
};

Spacecraft.prototype.listen = function (listener) {
    this.listeners.push(listener);
    return function () {
        this.listeners = this.listeners.filter(function (l) {
            return l !== listener;
        });
    }.bind(this);
};

module.exports = function () {
    return new Spacecraft()
};