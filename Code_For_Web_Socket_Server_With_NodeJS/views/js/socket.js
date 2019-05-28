var table = document.getElementById("tableData");
	var button = document.getElementById('btn');
	var led = document.getElementById('led');
	var pump = document.getElementById('pump');
	var fan = document.getElementById('fan');
	var control = document.getElementById('control');

	var batQuat = document.getElementById("batQuat");
	var batDen = document.getElementById("batDen");
	var tgNghi = document.getElementById("tgNghi");
	var tgBom = document.getElementById("tgBom");

	var text = document.getElementById('text');
	var url = window.location.host; // hàm trả về url của trang hiện tại kèm theo port
	var ws = new WebSocket('wss://' + url + '/ws'); // mở 1 websocket 
	//var ws = new WebSocket('wss://' + url);
	var pattTemp = new RegExp("^(temp)+");
	var pattHumd = new RegExp("^(humi)+");
	var pattLux = new RegExp("^(lux)+");

	var pattBatQuat = new RegExp("^(onQuat)+");
	var pattBatDen = new RegExp("^(onDen)+");
	var pattTgBom = new RegExp("^(tgBom)+");
	var pattTgNghi = new RegExp("^(tgNghi)+");

	var temp = null;
	var humd = null;
	var lux = null;

	var quatPara = null;
	var denPara = null;
	var tgBomPara = null;
	var tgNghiPara = null;

	
	ws.onopen = function() //khi websocket được mở thì hàm này sẽ được thưc hiện
	{
		document.getElementById('status').innerHTML = 'Connected';
		control.disabled = false;
		console.log('connected...')
	};
	ws.onmessage = function(evt) // sự kiện xảy ra khi client nhận dữ liệu từ server
	{
		console.log(evt.data)
		if(evt.data == 'LED_OFF') {
			led.checked = false;

		} else if(evt.data == 'LED_ON') {

			led.checked = true;

		}else if(evt.data == 'PUMP_OFF') {
			pump.checked = false;
		}else if(evt.data == 'PUMP_ON') {
			pump.checked = true;
		}
		else if(evt.data == 'FAN_OFF') {
			fan.checked = false;
		}else if(evt.data == 'FAN_ON') {
			fan.checked = true;
		}
		else if(pattTemp.test(evt.data) == true){
			var data = evt.data;
			temp = data.slice(5,data.length);
		}
		else if(pattHumd.test(evt.data) == true){
			var data = evt.data;
			humd = data.slice(5,data.length);
		}
		else if(pattLux.test(evt.data) == true){
			var data = evt.data;
			lux = data.slice(4,data.length);
		}
		else if(pattBatDen.test(evt.data) == true){
			var data = evt.data;
			quatPara = data.slice(6,data.length);
		}
		else if(pattBatQuat.test(evt.data) == true){
			var data = evt.data;
			denPara = data.slice(7,data.length);
		}
		else if(pattTgBom.test(evt.data) == true){
			var data = evt.data;
			tgBomPara = data.slice(6,data.length);
		}
		else if(pattTgNghi.test(evt.data) == true){
			var data = evt.data;
			tgNghiPara = data.slice(7,data.length);
		}
	};


	ws.onclose = function() { // hàm này sẽ được thực hiện khi đóng websocket
		led.disabled = true;
		fan.disabled = true;
		pump.disabled = true;
		control.disabled = true;
		document.getElementById('status').innerHTML = 'Connected';
	};
	led.onchange = function() { // thực hiện thay đổi bật/tắt led
		var led_status = 'LED_OFF';
		if (led.checked) {
			led_status = 'LED_ON';
		}
		ws.send(led_status);
	}

	pump.onchange = function() { // thực hiện thay đổi bật/tắt bom
		var pump_status = 'PUMP_OFF';
		if (pump.checked) {
			pump_status = 'PUMP_ON';
		}
		ws.send(pump_status);
	}

	fan.onchange = function() { // thực hiện thay đổi bật/tắt led
		var fan_status = 'FAN_OFF';
		if (fan.checked) {
			fan_status = 'FAN_ON';
		}
		ws.send(fan_status);
	}
	control.onchange = function(){
		led.disabled = !led.disabled;
		fan.disabled = !fan.disabled;
		pump.disabled = !pump.disabled;
		if(control.checked)ws.send("control");
		else ws.send("destroy");
	}

		window.onload = function() {
			var dataTemp = [];
			var dataHumd = [];
			var dataLux = [];
			var Chart = new CanvasJS.Chart("ChartContainer", {
				zoomEnabled: true, // Dùng thuộc tính có thể zoom vào graph
				title: {
					text: "Thông số Nhiệt độ và Độ ẩm" // Viết tiêu đề cho graph
				},
				toolTip: { // Hiển thị cùng lúc 2 trường giá trị nhiệt độ, độ ẩm trên graph
					shared: true
				},
				axisX: {
					title: "chart updates every 10 secs" // Chú thích cho trục X
				},
				data: [{
					// Khai báo các thuộc tính của dataTemp và dataHumd
					type: "line", // Chọn kiểu dữ liệu đường
					xValueType: "dateTime", // Cài đặt kiểu giá trị tại trục X là thuộc tính thời gian
					showInLegend: true, // Hiển thị "temp" ở mục chú thích (legend items)
					name: "temp",
					dataPoints: dataTemp // Dữ liệu hiển thị sẽ lấy từ dataTemp
				},
				{
					type: "line",
					xValueType: "dateTime",
					showInLegend: true,
					name: "humd",
					dataPoints: dataHumd
				}
				],
			});
			// do thi cho cuong do anh sang
			var Chart2 = new CanvasJS.Chart("ChartContainer2",{
				zoomEnabled: true, // Dùng thuộc tính có thể zoom vào graph
				title: {
					text: "Thông số cường độ ánh sáng" // Viết tiêu đề cho graph
				},
				toolTip: { // Hiển thị cùng lúc 2 trường giá trị nhiệt độ, độ ẩm trên graph
					shared: true
				},
				axisX: {
					title: "chart updates every 2 secs" // Chú thích cho trục X
				},
				data: [{
					// Khai báo các thuộc tính của dataTemp và dataHumd
					type: "line", // Chọn kiểu dữ liệu đường
					xValueType: "dateTime", // Cài đặt kiểu giá trị tại trục X là thuộc tính thời gian
					showInLegend: true, // Hiển thị "temp" ở mục chú thích (legend items)
					name: "cuong do anh sang",
					dataPoints: dataLux // Dữ liệu hiển thị sẽ lấy từ dataTemp
				}
				],
			});
			var yHumdVal = 0; // Biến lưu giá trị độ ẩm (theo trục Y)
			var yTempVal = 0; // Biến lưu giá trị nhiệt độ (theo trục Y)
			var yLuxVal = 0; //Biến lưu giá trị ánh sáng theo trục Y
			var updateInterval = 10000; // Thời gian cập nhật dữ liệu 2000ms = 2s
			var time = new Date(); // Lấy thời gian hiện tại
			var updateChart = function() {
				//httpGetAsync('/get', function(data) {
				// Gán giá trị từ localhost:8000/get vào textbox để hiển thị
					document.getElementById("temp").value = temp;
					document.getElementById("humd").value = humd;
					document.getElementById("lux").value = lux;
					batQuat.value = quatPara;
					batDen.value = denPara;
					tgNghi.value = tgNghiPara;
					tgBom.value = tgBomPara;
				// Xuất ra màn hình console trên browser giá trị nhận được từ localhost:8000/get
					//console.log(data);
// Cập nhật thời gian và lấy giá trị nhiệt độ, độ ẩm từ server
					time.setTime(time.getTime() + updateInterval);
					yTempVal = parseInt(temp);
					yHumdVal = parseInt(humd);
					yLuxVal = parseInt(lux);
					dataTemp.push({ // cập nhât dữ liệu mới từ server
						x: time.getTime(),
						y: yTempVal
					});
					dataHumd.push({
						x: time.getTime(),
						y: yHumdVal
					});
					dataLux.push({
						x: time.getTime(),
						y: yLuxVal
					});
					Chart.render(); // chuyển đổi dữ liệu của của graph thành mô hình đồ họa
					Chart2.render();
				//});
			};
			updateChart(); // Chạy lần đầu tiên
			setInterval(function() { // Cập nhật lại giá trị graph sau thời gian updateInterval
				updateChart();
			}, updateInterval);
		}

		$(".gallery").magnificPopup({
			delegate: 'a',
			type: 'image',
			gallery:{
				enabled: true
			} 
		});
		