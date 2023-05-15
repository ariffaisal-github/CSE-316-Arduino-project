const express = require('express');
const app = express();
const port = 3000;

app.get('/',function (req,res) {
    res.send('Hello world');
});
app.get('/auth/:rfid',((req, res) => {
    if(req.params.rfid === "e34a2d92"){
        res.send("not found");
    }
    else if(req.params.rfid === "821dae1a"){
        res.send("sifat");
    }
    else if(req.params.rfid === "1343b518"){
        res.send("arif");
    }
    else if(req.params.rfid === "b322b518"){
        res.send("PersonA");
    }
    else if(req.params.rfid === "d330a818"){
        res.send("PersonB");
    }else{
        res.send("not found");
    }
}));
app.listen(port,function () {
    console.log("Server is open on "+port);
});