const fs = require('fs');
const http = require('http');
const url = require('url');
const mime = require('mime');
const path = require('path');
const artTemplate = require('art-template');
const queryString = require('querystring');

const server = http.createServer();

require('./model/db.js');
const Student = require('./model/student.js');

server.on('request', async(req, res) => {
    let {pathname, query} = url.parse(req.url, true);
    let method = req.method;
    console.log('---------------');
    console.log('pathname:' + pathname);
    console.log('method:' + method);

    if(method === 'GET'){
        console.log(query);
        if(pathname === '/' || pathname === '/list' || pathname === '/list.html'){
            let tempStr = '';
            let students = await Student.find();
            students.forEach(element => {
                tempStr += '<tr>';
                tempStr += `<td>${element.name}</td>`;
                tempStr += `<td>${element.age}</td>`;
                tempStr += `<td>${element.password}</td>`;
                tempStr += `<td>${element.email}</td>`;
                tempStr += '<td>';
                element.courses.forEach(course => {
                    tempStr += course + ' ';
                });
                tempStr += '</td><td>';
                tempStr += `
                <a href="/delete?_id=${element._id}" class="MDButton">删除</a>
                <a href="/modify?_id=${element._id}" class="MDButton">修改</a>
                `;
                tempStr += '</td></tr>';
            });


            res.writeHead(200, {
                'content-type': 'text/html;charset=utf8'
            });

            let absPath = path.join(__dirname, '/page' , 'list.html');
            let pageContent = artTemplate(absPath, {
                studentInfo : tempStr
            });
            
            res.end(pageContent);
        }else if(pathname === '/add' || pathname === '/add.html'){
            let absPath = path.join('./page' , '/add.html');
            let pageContent = fs.readFileSync(absPath);               
            res.writeHead(200, {
                'content-type': 'text/html;charset=utf8'
            });
            res.end(pageContent);
        }else if(pathname === '/delete' || pathname === '/delete.html'){

            Student.findOneAndDelete(query, (err, data) => {
                if(err){
                    console.log('删除数据失败');
                }else{
                    console.log(data);
                };

                res.writeHead(302,{
                    'Location': '/list'
                });
                
                res.end();
            });

        }else if(pathname === '/modify' || pathname === '/modify.html'){

            Student.findOne(query, (err, data) => {
                if(err){
                    console.log('删除数据失败');
                }else{
                    console.log('修改数据查找');
                    console.log(data);
                    let absPath = path.join(__dirname, '/page' , 'modify.html');
                    let pageContent = artTemplate(absPath, {
                        modifyID : data._id,
                        modifyName : data.name,
                        modifyAge: data.age,
                        modifyPassWD: data.password,
                        modifyEmail: data.email,
                        CPP_course: "checked",
                        python_course: "checked",
                        js_course: "checked",
                        vue_course: "checked"
                    });
                    
                    res.end(pageContent);
                };              
            });

        }else{
            let absPath = path.join('./page' , pathname);
            if(fs.existsSync(absPath))
            {
                let type = mime.getType(absPath);
                console.log(type);
                let head = `${type};charset=utf8`;
                res.writeHead(200, {
                    'content-type': head
                });
                let pageContent = fs.readFileSync(absPath);
                res.end(pageContent);                
            }
            res.writeHead(404, {
                'content-type': 'text/html;charset=utf8'
            });
            res.end('<h1>内容找不到</h1>');
        }
    }else if(method === 'POST'){
        let postData = '';
        req.on('data', (rawData) => {
            postData += rawData;
        });
    
        req.on('end', ()=>{
            let dataObject = queryString.parse(postData);
            console.log(dataObject);

            if(pathname === '/postData'){
    
                Student.create(dataObject, (err, data) => {
                    if(err){
                        console.log('插入数据失败');
                    }else{
                        console.log(data);
                    }

                    res.writeHead(302,{
                        'Location': '/list'
                    })
                    
                    res.end();
                });
            }else if(pathname === '/modifyData'){

                let idObject = {_id:dataObject._id};
                delete dataObject._id;

                console.log(idObject);
                console.log(dataObject);

                Student.updateOne(idObject, dataObject,(err, data) => {
                    if(err){
                        console.log('修改数据失败');
                        console.log(err);
                    }else{
                        console.log(data);
                    }

                    res.writeHead(302,{
                        'Location': '/list'
                    })
                    
                    res.end();
                });
    
            }else{
                res.writeHead(200, {
                    'content-type': 'text/html;charset=utf8'
                });
                res.end('<h1>内容找不到</h1>');
            }
        });

    }
});

server.listen(8877);


