import React from 'react';
import NavBar, { BreadCrumb } from "../components/NavBar";
import { Container } from 'reactstrap';
import { useLocation } from 'react-router-dom'
import * as utils from '../utils'


export default props => {
    const { pathname } = useLocation();

    // generate breadcrumb list of items from the current path
    const routeParts = pathname.split("/").filter(Boolean);
    const breadCrumbItems = [{href: "/", text: "Home"}];
    let tempHref = "";
    for (let i in routeParts)
    {
        tempHref += `/${routeParts[i]}`;

        breadCrumbItems.push({
            href: tempHref,
            text: routeParts[i].split("-").map(utils.capitalize).join(" "),
        });
    }

    return (
        <React.Fragment>
            <NavBar user={props.user}/>
            <div id="content">
                <main>
                    <Container>
                        <BreadCrumb breadcrumbItemsList={breadCrumbItems} />
                        {props.children}
                    </Container>
                </main>
                <Container id="footer">
                    All rights reserved &copy;2020
                </Container>
            </div>
        </React.Fragment>
    );
};