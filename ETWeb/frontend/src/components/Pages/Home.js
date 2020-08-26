import React from 'react';
import { Jumbotron, Container } from 'reactstrap';

export default props => {
    return (
        <Container>
            <Jumbotron fluid>
                <Container id="jumbo" fluid>
                  <h1 className="display-3">Employee Tracker</h1>
                  <p className="lead">Free open source computing software
                      that will let you monitor activity of your employees</p>
                </Container>
            </Jumbotron>

            <div className="paragraph-block">
                <h4>Track the status of your remote workforce</h4>
                <p>
                    Thanks to Employee Tracker, you can monitor the status of your employees:
                    online if they are actively using the internet, idle if they are on a break
                    and have been inactive for a certain period of time, and offline if they
                    are not connected to network. This way, you can keep track of your team’s productivity
                    and ensure your staff is active during working hours.
                </p>
            </div>
            <div className="paragraph-block">
                <h4>Monitor recent activity of your employees</h4>
                <p>
                    Employee Tracker takes a screenshot of an employee’s desktop and sends
                    it to the server every 15 min; you can, thus, easily see a worker’s
                    real-time activities and view websites, programs, applications that
                    they are accessing.
                </p>
            </div>
            <div className="paragraph-block">
                <h4>Browse an employee’s network activity</h4>
                <p>
                    Employee Tracker allows you to obtain the report on an employee’s browsing history, with the names of
                    the most searched websites and the total number of visits. The data is retrieved hourly and is formed
                    in an easy-to-read infographics that you can later use for reports and presentations.
                </p>
            </div>
        </Container>
    );
};