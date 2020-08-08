import React from 'react';
import NavBar, { BreadCrumb } from "../components/NavBar";
import { Container } from 'reactstrap';
import { useLocation } from 'react-router-dom'
import * as utils from '../utils'


export default props => {
    const { pathname } = useLocation();
    const routeParts = pathname.split("/").filter(Boolean);
    const breadCrumbItems = [{href: "/", text: "Home"}];
    let tempHref = "";
    for (let i in routeParts)
    {
        tempHref += `/${routeParts[i]}`;

        breadCrumbItems.push({
            href: tempHref,
            text: utils.capitalize(routeParts[i]),
        });
    }

    return (
        <React.Fragment>
            <NavBar user={props.user}/>
            <Container id="main">
                <BreadCrumb breadcrumbItemsList={breadCrumbItems} />
                {props.children}
            </Container>
            <Container className="" id="footer">
                All rights reserved &copy;2020
            </Container>
        </React.Fragment>
    );
};