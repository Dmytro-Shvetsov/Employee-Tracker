import React from 'react';
import { Jumbotron, Container } from 'reactstrap';

export default props => {
    return (
        <div className="container">
            <Jumbotron fluid>
                <Container id="jumbo" fluid>
                  <h1 className="display-3">Employee Tracker</h1>
                  <p className="lead">Free open source computing software
                      that will let you monitor activity of your employees.</p>
                </Container>
            </Jumbotron>
        </div>
    );
};