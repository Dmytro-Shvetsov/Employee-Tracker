import React from 'react';
import NavBar from "../components/NavBar";
import { Container } from 'reactstrap';

export default props => {
    return (
        <React.Fragment>
            <NavBar user={props.user}/>
            <Container className="container" style={{minHeight:500}}>
                {props.children}
            </Container>
            <Container className="" container id="footer">
                All rights reserved &copy;2020
            </Container>
        </React.Fragment>
    );
};