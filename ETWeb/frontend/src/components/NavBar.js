import React from 'react';
import {
  Collapse,
  Button,
  Navbar,
  NavbarToggler,
  NavbarBrand,
  Nav,
  NavItem,
  UncontrolledDropdown,
  DropdownToggle,
  DropdownMenu,
  DropdownItem,
  NavbarText
} from 'reactstrap';
import { Link } from 'react-router-dom';


const AuthItems = props => {
    const { user, ...rest } = props;
    console.log(props);
    if (user) {
        console.log('User is authenticated: ', user);
        return (
            <React.Fragment>
                <NavItem key="dashboard">
                    <Link className="nav-link" to="/dashboard">Profile</Link>
                </NavItem>
                <NavItem key="logout">
                    <Link className="nav-link" to="/auth/logout">Log Out</Link>
                </NavItem>
            </React.Fragment>
        );
    }
    return (
        <React.Fragment>
            <Button color="light" size="md" key="login" className="auth-btn ">
                <Link className="text-dark nav-link" to="/auth/login">Log In</Link>
            </Button>
            <Button color="light" key="signup" className="auth-btn">
                <Link className="text-dark nav-link" to="/auth/register">Sign Up</Link>
            </Button>
        </React.Fragment>
    );
};

class NavBar extends React.Component {
    constructor(props) {
        super(props)
    }
    handleSelect = (selectedElem) => {
        console.log(selectedElem)
    };
    render() {
        return (
            <Navbar color="dark" expand="md">
                <Collapse navbar>
                    <Nav className="m-auto"  id="navbar" navbar>
                        {/*<NavbarBrand href="/">Employee Tracker</NavbarBrand>*/}
                        <NavItem active key="features">
                            <Link to="/features" className="nav-link text-light">Features</Link>
                        </NavItem>
                        <NavItem>
                            <Link to="/idea" className="nav-link text-light">How it works</Link>
                        </NavItem>
                        <NavItem>
                            <Link to="/benefits" className="nav-link text-light">Benefits</Link>
                        </NavItem>
                        <NavItem>
                            <Link to="/contact" className="nav-link text-light">Contact Us</Link>
                        </NavItem>
                        <div id="auth-items">
                            <AuthItems user={this.props.user}/>
                        </div>
                    </Nav>
                </Collapse>
            </Navbar>
        );
    }
}

export default NavBar;