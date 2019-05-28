var express = require("express");
var app = express();
var WebSocket = require('ws');
var path = require("path");
var session = require('express-session');
var assert = require('assert');
var bodyParser=require('body-parser');
var nodeExcel = require('excel-export');
var dbo;

var MongoClient = require('mongodb').MongoClient;
var url = "mongodb+srv://theanhpham:theanh07111997@sensordata-qnm4f.mongodb.net/test?retryWrites=true";
MongoClient.connect(url,{useNewUrlParser: true}, function(err, db) {
  
  			if (err) throw err;
  			dbo = db.db("HTTrongRauIOT");
 });

app.use(express.static(path.join(__dirname, '/')));
app.set("view engine", "ejs");
app.set("views", "./");
app.use(bodyParser.urlencoded({extended:true}));
app.use(bodyParser.json());
app.use(session({
	secret: 'secret',
	resave: true,
	saveUninitialized: true
}));


//app.listen(process.env.PORT || 3000);
app.get('/getData', (req, res) => {
  	dbo.collection('Dinh_Duong_Cay_Trong').find().toArray((err, result) => {
    	if (err) return console.log(err);
    	res.send(result);
  	});
});

app.get('/create-link',function(req,res){
	var conf = {};
	conf.cols = [
		{caption:'col1', type: 'string'},
		{caption: 'col2', type: 'string'},
		{caption: 'col3', type: 'string'},
		{caption: 'col4', type: 'string'},
		{caption: 'col5', type: 'string'}
	];

	var users = dbo.collection('Dinh_Duong_Cay_Trong');
        users.find({}).toArray(function (err,results) {
            if (err) throw err;
            results.forEach(function (row) {
               conf.rows.push(row);
               console.log(conf.rows);
            });

            /*var result = nodeExcel.execute(conf);
       		res.setHeader('Content-Type', 'application/vnd.openxmlformats');
       		res.setHeader("Content-Disposition", "attachment; filename=" + "Report.xlsx");
       		res.end(result, 'binary');*/
        });
});


app.get("/get-data-dinh-duong",function(req,res){
	var collect = dbo.collection('Dinh_Duong_Cay_Trong');
        collect.find({}).toArray(function (err,results) {
            if (err) throw err;
            data = '<table border="1" style="border-collapse:collapse" cellspacing="5" cellpadding="15">';
            data += '<tr><th>Name</th><th>PH</th><th>Light</th><th>PPM</th><th>ML</th></tr>';
            results.forEach(function (row) {
                data += '<tr>';
                data += '<td>' + row.Name + '</td>';
                data += '<td>' + row.PH + '</td>';
                data += '<td>' + row.Light + '</td>';
                data += '<td>' + row.PPM + '</td>';
                data += '<td>' + row.ML + '</td>';
                data += '</tr>';
            });
            data += '</table>';
            res.writeHead(200, {'Content-Type': 'text/html; charset=utf-8'});
            res.end(data);
        })
       // db.close();
});

app.get("/get-data-humd",function(req,res){
	var doAmTrungBinh = 0;
	var dataNumber = 0;
	var minHumd=1000;
	var maxHumd=0;
	var collect = dbo.collection('humdData');
        collect.find({}).toArray(function (err,results) {
            if (err) throw err;
            data = '<table border="1" style="border-collapse:collapse" cellspacing="5" cellpadding="15">';
            data += '<tr><th>Data</th><th>Time</th></tr>';
            results.forEach(function (row) {
                data += '<tr>';
                data += '<td>' + row.data + '</td>';
                data += '<td>' + row.time + '</td>';
                data += '</tr>';
                doAmTrungBinh += parseInt(row.data, 10);
              
                dataNumber ++;
                if(parseInt(row.data,10) > maxHumd) maxHumd = parseInt(row.data,10);
                if(parseInt(row.data,10) < minHumd) minHumd = parseInt(row.data,10);
            });
            doAmTrungBinh = parseInt(doAmTrungBinh/dataNumber,10);
            dataDetail = '<h3 style="color:red">Độ ẩm trung bình là:  '+doAmTrungBinh+'</h3>';
            dataDetail += '<h3 style="color:red">Độ ẩm cao nhất la:  '+maxHumd+'</h3>';
            dataDetail += '<h3 style="color:red">Độ ẩm thấp nhất là:  '+minHumd+'</h3>';
            data += '</table>';
            dataDetail +=data;
            res.writeHead(200, {'Content-Type': 'text/html; charset=utf-8'});
            res.end(dataDetail);
        })
       // db.close();
});

app.get("/get-data-temp",function(req,res){
	var nhietDoTrungBinh = 0;
	var dataNumber = 0;
	var minTemp=1000;
	var maxTemp=0;
		var collect = dbo.collection('tempData');
        collect.find({}).toArray(function (err,results) {
            if (err) throw err;
            data = '<table border="1" style="border-collapse:collapse" cellspacing="5" cellpadding="15">';
            data += '<tr><th>Data</th><th>Time</th></tr>';
            results.forEach(function (row) {
                data += '<tr>';
                data += '<td>' + row.data + '</td>';
                data += '<td>' + row.time + '</td>';
                data += '</tr>';
                nhietDoTrungBinh += parseFloat(row.data, 10);
               // console.log(nhietDoTrungBinh);
                dataNumber ++;
                if(parseFloat(row.data,10)>maxTemp)maxTemp=parseFloat(row.data,10);
                if(parseFloat(row.data,10)<minTemp)minTemp=parseFloat(row.data,10);
            });
            nhietDoTrungBinh = parseInt(nhietDoTrungBinh/dataNumber,10);
            dataDetail = '<h3 style="color:red">Nhiệt độ trung bình là:  '+nhietDoTrungBinh+'</h3>';
            dataDetail += '<h3 style="color:red">Nhiệt độ cao nhất la:  '+maxTemp+'</h3>';
            dataDetail += '<h3 style="color:red">Nhiệt độ thấp nhất là:  '+minTemp+'</h3>';
            data += '</table>';
            dataDetail+=data
            res.writeHead(200, {'Content-Type': 'text/html; charset=utf-8'});
            res.end(dataDetail);
        })
       // db.close();
});

app.get("/get-data-lux",function(req,res){
	var cuongDoASTrungBinh = 0;
	var dataNumber = 0;
	var minLux=3000;
	var maxLux=0;
	var collect = dbo.collection('luxData');
        collect.find({}).toArray(function (err,results) {
            if (err) throw err;
            data = '<table border="1" style="border-collapse:collapse" cellspacing="5" cellpadding="15">';
            data += '<tr><th>Data</th><th>Time</th></tr>';
            results.forEach(function (row) {
                data += '<tr>';
                data += '<td>' + row.data + '</td>';
                data += '<td>' + row.time + '</td>';
                data += '</tr>';
                cuongDoASTrungBinh += parseInt(row.data, 10);
              
                dataNumber ++;
                if(parseInt(row.data,10) > maxLux) maxLux = parseInt(row.data,10);
                if(parseInt(row.data,10) < minLux) minLux = parseInt(row.data,10);
            });
            cuongDoASTrungBinh = parseInt(cuongDoASTrungBinh/dataNumber,10);
            dataDetail = '<h3 style="color:red">Cường độ ánh sáng trung bình là:  '+cuongDoASTrungBinh+'</h3>';
            dataDetail += '<h3 style="color:red">Cường độ ánh sáng cao nhất là:  '+maxLux+'</h3>';
            dataDetail += '<h3 style="color:red">Cường độ ánh sáng thấp nhất là:  '+minLux+'</h3>';
            data += '</table>';
            dataDetail += data;
            res.writeHead(200, {'Content-Type': 'text/html; charset=utf-8'});
            res.end(dataDetail);
        })
       // db.close();
});

app.get("/trangchu",function(req,res){
	if(req.session.loggedin){
		res.render("views/index");
	}
	else{
		res.render("views/login");
	}	
});

app.get("/",function(req,res){
	if(req.session.loggedin){
		res.render("views/index");
	}
	else{
		res.render("views/login");
	}	
});

app.get("/updateUser",function(req,res){
	res.render("views/updateUser")
});

app.get("/error",function(req,res){
	res.render("views/error");
});

app.get("/login",function(req,res){
	res.render("views/login");
});

app.get("/logout",function(req,res){
	req.session.destroy(function (err) {
      if (err) {
        throw err;
      } else {
        res.redirect('/');
      }
    });
});


app.get("/index",function(req,res){
	if (req.session.loggedin) {
		res.writeHead(200, {'Content-Type': 'text/html; charset=utf-8'});
		data = '<head><link rel="stylesheet" href="views/css/styleManageUser.css"><meta name="viewport" content="width=device-width, initial-scale=1.0">';
		data += '<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">';
		data += '<title>Quan Ly Tai Khoan</title></head>';
		data += '<body><h1>Hello ' + req.session.username + '!</h1>'
		data += '<div class="grid-container"><p><i class="fas fa-sign-in-alt"></i><a href="trangchu">Vào trang quản trị</a></p>';
		data += '<p><i class="fas fa-sign-out-alt"></i><a href="logout">Log out</a></p>';
		data += '<p><i class="fas fa-user-edit"></i><a href="updateUser">Cập nhật tài khoản quản trị</a></p></div></body>';
		res.end(data);
	} else {
		res.send('Please login to view this page!');
	}
	
});

//xu ly update mat khau
app.post('/update', function(request, response) {
	var oldPassword = request.body.oldPassword;
	var newPassword = request.body.newPassword;
	var name = request.session.username;
	if (oldPassword && newPassword) {

  			var query = {username:name,password:oldPassword};
  			var newvalues = { $set: { password: newPassword } };
  			dbo.collection("user").updateOne(query, newvalues, function(err, result){
  				//if(err) throw err;
  				if (err) {
  					response.redirect("/error");
				} else {
					response.redirect("/logout");
				}			
				response.end();
		});
	} else {
		response.send('Please enter old Password and new Password!');
		response.end();
	}
});

//xu ly dang nhap
app.post('/auth', function(request, response) {
	var name = request.body.username;
	var pass = request.body.password;
	if (name && pass) {

  			var query = {username:name,password:pass};
  			dbo.collection("user").find(query).toArray(function(err,result){
  				//if(err) throw err;
  				if (result.length > 0) {
					request.session.loggedin = true;
					request.session.username = name;
					response.redirect('/index');
				} else {
					//response.send('Incorrect Username and/or Password!');
					response.redirect("/error");
				}			
				response.end();
  				//db.close();
  			
		});
	} else {
		response.send('Please enter Username and Password!');
		response.end();
	}
});

var ws = new WebSocket.Server({server:app.listen(process.env.PORT || 3000)});
var clients = [];

function broadcast(socket,data){
	console.log(clients.length);
	for(var i=0; i<clients.length;i++){
		if(clients[i] != socket){
			clients[i].send(data);
		}
	}
}

function insertData(database,data){
	var myobj = { data: data.data, time: data.time };
  	dbo.collection(database).insertOne(myobj, function(err, res) {
    	if (err) throw err;
    	console.log("1 document inserted");
    	//db.close();
  	});
}

var pattTemp = new RegExp("^(temp)+");
var pattHumd = new RegExp("^(humi)+");
var pattLux = new RegExp("^(lux)+");

ws.on('connection',function(socket,req){
	clients.push(socket);

	socket.on('message', function(message){
		console.log("received: %s", message);
		broadcast(socket,message);
		if(pattTemp.test(message) == true){
			var database = "tempData"
			var data = message;
			var temp = data.slice(5,message.length);
			var newData = {
				data: temp,
				time: new Date()
			};
			insertData(database,newData);
		}
		else if(pattHumd.test(message) == true){
			var data = message;
			var database = "humdData"
			var humd = data.slice(5,message.length);
			var newData = {
				data: humd,
				time: new Date()
			};
			insertData(database,newData);
		}
		else if(pattLux.test(message) == true){
			var data = message;
			var database = "luxData"
			var lux = data.slice(5,message.length);
			var newData = {
				data: lux,
				time: new Date()
			};
			insertData(database,newData);
		}
		

	});

	socket.on('close', function(){
		var index = clients.indexOf(socket);
		clients.splice(index,1);
		console.log('disconnected');
	});
});