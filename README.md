## Employee Tracker - software for tracking productivity of remote employees

The project is comprised of desktop(client) and web applications. The first one is used to collect data from employees when they are working, whereas the other one is for monitoring 
the gathered data. Built with <code>Qt, Django(Channels, REST framework)</code> and <code>React</code>.

## Project Status

This project is currently in development.

### Features:
#### Client application
  - [x] Authorization     
  - [x] Websocket connection for real time data transfers
  - [x] Internet traffic analysis(HTTP/SSL/TLS packets)  
  - [x] Screen shots collection 
  - [ ] Idle prompt and timeout
  - [ ] Tracking how much time the employee has worked as for particular day shift
  
#### Web application
  - [x] Authorization 
  - [x] User profile/account pages 
  - [x] Your projects page (for staff members only). Ability to create new projects and view existing ones. Still cannot add new employees
  - [x] Real time updates when an employee's working status(online, offline, etc) changes. Can be observed in project's detailed view
layer of it, application protocols and means to implement this knowledge in practice

At the end of the day, the technologies implemented in this project are <code>Qt, Django(with Channels and REST framework), Webpack, React, React-Router 5.0, Axios, HTML, CSS(Reactstrap)</code>. 
