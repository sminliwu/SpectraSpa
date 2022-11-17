// web portal functionality

var aShuffle = {
	0: false,
	1: false,
	2: false,
	3: false,
	4: false,
	5: false,
	6: false
}

var bShuffle = {
	0: false,
	1: false,
	2: false,
	3: false,
	4: false,
	5: false,
	6: false
}

var cShuffle = {
	0: false,
	1: false,
	2: false,
	3: false,
	4: false,
	5: false,
	6: false
}

$(document).ready(function() {

	// CONTROL C STATUS DISPLAYS
	function upCSCurrent(str) {
		$("#cst").html(str);
	}

	function upCSShuffle(x) {
		cShuffle[x] = !cShuffle[x];
		if (cShuffle[x]) {
			$("#css"+x).html("on");
		} else {
			$("#css"+x).html("off");
		}
	}

	var ws = new WebSocket('ws://'+ location.hostname +':81/', ['arduino']);
	ws.onopen = function () {
		ws.send('Connect ' + new Date());
	};
	ws.onerror = function (error) {
		console.log('WebSocket Error ', error);
	};
	ws.onmessage = function (e) {
		parseMessage(String(e.data));
	};
	ws.onclose = function () {console.log('WebSocket connection closed');
	};

	for (var i=0; i<8; i++) {
		$('#css'+i).click((e) => {
			ws.send('?css'+i);
		});
	}

	$('#send-cmd').click((e) => {
		ws.send($('#cmd').value());
		$('#cmd').value("");
	});
});

function parseMessage(str) {
	console.log(str);
	$('#msg').innerHTML(str);
	if ($('#'+str.substring(0, 4))) {
		$('#'+str.substring(0, 4)).innerHTML(str.substring(4));
	} else if ($('#'+str.substring(0, 2))) {
		$('#'+str.substring(0, 2)).innerHTML(str.substring(2));
	}
}