const { exec, spawn } = require('child_process')
const io = require('socket.io-client')
require('dotenv').config()

const express = require('express')
const app = express()
app.use(express.static(__dirname, { dotfiles: 'allow' } ))

// exec('python3 ./python/iotReceive.py')

// app.get('/:deviceId/:command', (req, res) => {
//   try {
//     spawn('python3', ['./python/iotSend.py', req.params.deviceId, req.params.command])
//     res.sendStatus(200)
//   } catch(e) {
//     res.status(500).send(e)
//   }
// })

app.get('/', (req, res) => {
  // const { CLIENT_ID, TOKEN } = process.env
  // const connection = io.connect(`http://localhost:8081/${CLIENT_ID}?token=${TOKEN}`)
  const connection = io.connect(`http://localhost:8081`)
  connection.emit('toServer', 'hello from pi')
  connection.on('fromServer', data => {
    console.log(data)
  })

  res.send(`
    <html>
      <body>
        <h4>Hello from Pi</h4>
      </body>
    </html>
  `)
})

app.listen(8082, () => {
  console.log('Server Started: http://localhost:8082')
})

// const { CLIENT_ID, TOKEN } = process.env
// const connection = io.connect(`http://localhost:8081/${CLIENT_ID}?token=${TOKEN}`)
const connection = io.connect(`http://localhost:8081`)
connection.emit('toServer', 'hello from pi')
connection.on('fromServer', data => {
  console.log(data, connection.id)
})
