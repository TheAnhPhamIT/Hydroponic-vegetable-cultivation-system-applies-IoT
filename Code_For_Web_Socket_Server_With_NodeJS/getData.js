//var http = require('http');
var mongoClient = require('mongodb').MongoClient;

var url = "mongodb+srv://theanhpham:theanh07111997@sensordata-qnm4f.mongodb.net/test?retryWrites=true";



    mongoClient.connect(url,{useNewUrlParser: true}, function(err, db) {
        if (err) throw err;
        dbo = db.db("HTTrongRauIOT");
        var users = dbo.collection('user');
        var query = {username:"admin",password:"12345"};
        var newvalues = { $set: { password: "12345678" } };
        users.updateOne(query, newvalues, function(err, result){
            if (err) throw err;
            console.log(result);
            db.close();
        });
           
       // db.close();
    });

