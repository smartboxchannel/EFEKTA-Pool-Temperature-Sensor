const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const extend = require('zigbee-herdsman-converters/lib/extend');
const e = exposes.presets;
const ea = exposes.access;

const tzLocal = {
    node_config: {
        key: ['report_delay', 'comparison_previous_data', 'led_illumination', 'led_freq', 'led_duration'],
        convertSet: async (entity, key, rawValue, meta) => {
            const lookup = {'OFF': 0x00, 'ON': 0x01};
			const endpoint = meta.device.getEndpoint(1);
            const value = lookup.hasOwnProperty(rawValue) ? lookup[rawValue] : parseInt(rawValue, 10);
            const payloads = {
                report_delay: ['genBasic', {0x0201: {value, type: 0x21}}],
				comparison_previous_data: ['genBasic', {0x0205: {value, type: 0x10}}],
				led_illumination: ['genBasic', {0x0600: {value, type: 0x10}}],
				led_freq: ['genBasic', {0x0601: {value, type: 0x21}}],
				led_duration: ['genBasic', {0x0602: {value, type: 0x21}}],
            };
            await endpoint.write(payloads[key][0], payloads[key][1]);
            return {
                state: {[key]: rawValue},
            };
        },
    },
};

const fzLocal = {
    node_config: {
        cluster: 'genBasic',
        type: ['attributeReport', 'readResponse'],
        convert: (model, msg, publish, options, meta) => {
            const result = {};
            if (msg.data.hasOwnProperty(0x0201)) {
                result.report_delay = msg.data[0x0201];
            }
			if (msg.data.hasOwnProperty(0x0205)) {
				result.comparison_previous_data = ['OFF', 'ON'][msg.data[0x0205]];
            }
			if (msg.data.hasOwnProperty(0x0600)) {
				result.led_illumination = ['OFF', 'ON'][msg.data[0x0600]];
            }
			if (msg.data.hasOwnProperty(0x0601)) {
				result.led_freq = msg.data[0x0601];
            }
			if (msg.data.hasOwnProperty(0x0602)) {
				result.led_duration = msg.data[0x0602];
            }
            return result;
        },
    },
};

const definition = {
        zigbeeModel: ['Pool_Temperature_smart_sensor'],
        model: 'Pool_Temperature_smart_sensor',
        vendor: 'Custom devices (DiY)',
        description: 'Temperature and light smart sensor for swimming pool',
        fromZigbee: [fz.battery, fzLocal.node_config, fz.temperature, fz.illuminance],
        toZigbee: [tz.factory_reset, tzLocal.node_config],
        configure: async (device, coordinatorEndpoint, logger) => {
			const endpointOne = device.getEndpoint(1);
			await reporting.bind(endpointOne, coordinatorEndpoint, ['genBasic', 'genPowerCfg', 'msTemperatureMeasurement', 'msIlluminanceMeasurement']);
        },
        exposes: [e.battery(), e.battery_low(), e.battery_voltage(), e.temperature(), e.illuminance(), e.illuminance_lux(),
		exposes.numeric('report_delay', ea.STATE_SET).withUnit('Seconds').withDescription('Adjust Report Delay. Setting the time in seconds, by default 60 seconds')
            .withValueMin(20).withValueMax(3600),
		exposes.binary('comparison_previous_data', ea.STATE_SET, 'ON', 'OFF').withDescription('Enable сontrol of comparison with previous data'),
		exposes.binary('led_illumination', ea.STATE_SET, 'ON', 'OFF').withDescription('Enable сontrol of LED'),
		exposes.numeric('led_freq', ea.STATE_SET).withUnit('Seconds').withDescription('LED operation interval').withValueMin(1).withValueMax(3600),
		exposes.numeric('led_duration', ea.STATE_SET).withUnit('Milliseconds').withDescription('LED flash duration').withValueMin(1).withValueMax(5000)],
};

module.exports = definition;