const { spawn } = require('child_process')
const express = require('express')
const app = express()

app.get('/:deviceId/:command', (req, res) => {
  try {
    spawn('python3', ['./python/iotSend.py', req.params.deviceId, req.params.command])
    res.sendStatus(200)
  } catch(e) {
    res.status(500).send(e)
  }
})

app.listen(8082, () => console.log('Server Started'))
