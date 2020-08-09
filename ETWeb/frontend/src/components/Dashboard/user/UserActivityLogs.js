import React from 'react';
import { TabContent, TabPane, Nav, NavItem, NavLink, Row, Col } from 'reactstrap';


export default class UserActivityLogs extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            activeTab: 0
        }
    }

    render() {
        console.log(this.props.match);
        return (
            <div>
                <Nav tabs>
                    <NavItem>
                        <NavLink
                            className="active"
                            onClick={() => {
                                toggle('1');
                            }}
                        >
                            Tab1
                        </NavLink>
                    </NavItem>
                    <NavItem>
                        <NavLink
                            className=""
                            onClick={() => {
                                toggle('2');
                            }}
                        >
                            More Tabs
                        </NavLink>
                    </NavItem>
                </Nav>
                <TabContent activeTab={"2"}>
                    <TabPane tabId="1">
                        <Row>
                            <Col sm="12">
                                <h4>Tab 1 Contents</h4>
                            </Col>
                        </Row>
                    </TabPane>
                    <TabPane tabId="2">
                        tab2
                    </TabPane>
                </TabContent>
            </div>
        );
    }

}
