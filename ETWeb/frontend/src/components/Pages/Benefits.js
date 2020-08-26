import React from 'react';
import { DollarCircleOutlined, CommentOutlined } from '@ant-design/icons';
import { CardHeader, Container, Card, CardTitle, CardText } from 'reactstrap';

export default props => {
    return (
        <Container className="text-center px-3">
            <Card
                body inverse
                className="benefit-card"
            >
                <CardHeader className="text-center">
                    <DollarCircleOutlined className="header-card-icon"/>
                </CardHeader>
                <CardTitle>For Business Owners</CardTitle>
                <CardText>
                    Use the application to monitor your employees’ productivity:
                    track recent activity of your workforce to ensure your project is running
                    smoothly and no employee-related problems arise on the way.
                </CardText>
            </Card>
            <Card
                body
                inverse
                className="benefit-card"
            >
                <CardHeader className="text-center">
                    <CommentOutlined className="header-card-icon"/>
                </CardHeader>
                <CardTitle>For Freelancers</CardTitle>
                <CardText>
                    Keep track of your own working activities and take advantage of Employee Tracker reports
                    to have a concrete proof of time spent when billing clients.
                </CardText>
            </Card>
        </Container>
    );
};