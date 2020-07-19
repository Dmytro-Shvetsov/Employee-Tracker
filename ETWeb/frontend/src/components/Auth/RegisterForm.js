import { registerUser } from '../../services/authService.js'
import React from 'react';
import { Button, Form, FormGroup, Label, Input, FormFeedback } from 'reactstrap';
import { Container } from "reactstrap";
import { Link } from 'react-router-dom';

class RegisterForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            username: '',
            email: '',
            password: '',
            confirm_password: '',
            is_staff: false,
            errors: {}
        };
    }

    handleUsernameChange = (event) => {
        this.setState({
            ...this.state,
            errors: {...this.state.errors, username: undefined},
            username: event.target.value
        });
        console.log(this.state)
    };

    handleEmailChange = (event) => {
        this.setState({
            ...this.state,
            errors: {...this.state.errors, email: undefined},
            email: event.target.value
        });
    };

    handlePasswordChange = (event) => {
        this.setState({
            ...this.state,
            errors: {...this.state.errors, password: undefined},
            password: event.target.value
        });
    };

    handleConfirmPasswordChange = (event) => {
        this.setState({
            ...this.state,
            errors: {...this.state.errors, confirm_password: undefined},
            confirm_password: event.target.value
        });
    };
    handleIsStaffChange = (event) => {
        this.setState({
            ...this.state,
            is_staff: !this.state.is_staff,
            errors: {...this.state.errors}
        });
    };

    handleSubmit = (event) => {
        event.preventDefault();

        registerUser(this.state).then((res) => {
            const { user } = JSON.parse(res.data);
            console.log(user);
            alert('successful registration')
            // this.props.onRegister();
        }).catch((error) => {
            if (error.response.status === 401) {
                const fieldErrors = error.response.data;

                Object.keys(fieldErrors).map((fieldName) => {
                    fieldErrors[fieldName] = fieldErrors[fieldName].join(' ');
                });

                this.setState({
                    ...this.state,
                    errors: fieldErrors
                });
            } else {
                console.log("Unexpected error occurred. ", error);
            }
        });
    };

    render() {
        const { errors } = this.state;
        return (
            <React.Fragment>
                <Container className="col-8" style={{margin: "auto"}}>
                    <h1>Account Sign in</h1>
                    <p>
                        Use the form below to sign in to your online account.
                        Please enter your information exactly as given to you.
                        If you are having trouble please use the 'forgot username/password'
                        link or <Link to = "/contact">contact our staff</Link> for assistance.
                    </p>
                </Container>
                <Form className="auth-form" autocomplete="on">
                    <FormGroup>
                        <Label for="username">Username</Label>
                        <Input invalid={errors.username !== undefined}
                               onChange={this.handleUsernameChange}
                               type="text" name="username"
                               id="username"
                               placeholder="*"/>
                        <FormFeedback>{errors.username}</FormFeedback>
                    </FormGroup>
                    <FormGroup>
                        <Label for="email">Email</Label>
                        <Input invalid={errors.email !== undefined}
                               onChange={this.handleEmailChange}
                               type="email"
                               name="email"
                               id="email"
                               placeholder="*"/>
                        <FormFeedback>{errors.email}</FormFeedback>
                    </FormGroup>
                    <FormGroup>
                        <Label for="password">Password</Label>
                        <Input invalid={errors.password !== undefined}
                               onChange={this.handlePasswordChange}
                               type="password"
                               name="password"
                               id="password"
                               placeholder="*"/>
                        <FormFeedback>{errors.password}</FormFeedback>
                    </FormGroup>
                    <FormGroup>
                        <Label for="confirm_password">Confirm Password</Label>
                        <Input invalid={errors.confirm_password !== undefined}
                               onChange={this.handleConfirmPasswordChange}
                               type="password"
                               name="confirm_password"
                               id="confirm_password"
                               placeholder="*"/>
                        <FormFeedback>{errors.confirm_password}</FormFeedback>
                    </FormGroup>
                    <FormGroup check>
                        <Label check>
                            <Input type="checkbox" onChange={this.handleIsStaffChange}/>{' '}
                            I'd like to create a manager account
                        </Label>
                    </FormGroup>
                    <FormGroup className="auth-other">
                        {/*<Label>*/}
                        {/*    <Link to="/auth/reset">Forgot your username or password?</Link>*/}
                        {/*</Label>*/}
                        {/*<br />*/}
                        <Button onClick={(event) => this.handleSubmit(event)}>Submit</Button>
                    </FormGroup>
                </Form>
            </React.Fragment>
        );
    }
}

export default RegisterForm;
