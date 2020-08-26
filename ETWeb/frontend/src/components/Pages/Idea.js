import React from 'react';
import { Container } from 'reactstrap';

export default props => {
    return (
        <Container>
            <div className="paragraph-block">
                <p>
                    Employee Tracker is a computer service monitoring the activity of your employees from anywhere, anytime.
                    The service is absolutely free of charge; the only thing you have to do is to log in, set your personal
                    account, add your project details and information on employees&nbsp;- and off you go to a great start!
                    Watch this short demo video to find out how the service works.
                </p>
            </div>
            <div className="paragraph-block">
                <iframe id="demo" src="https://www.youtube.com/embed/7NNHPWsFx0A"/>
            </div>
        </Container>
    );
};