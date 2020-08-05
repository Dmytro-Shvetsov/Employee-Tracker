import React from 'react';
import { Button, Form, FormGroup, Label, Input, Alert, Container } from 'reactstrap';
import { Link, Redirect } from 'react-router-dom';
import TextInput from '../common/Input'
import { loginUser } from "../../services/authService";


export default class LoginForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            data: {
                username: '',
                password: '',
            },
            rememberMe: false,
            errors: {},
            loginFinished: false
        };
    }

    handleInputChange = event => {
        const target = event.target;
        const name = target.name;

        this.setState({
            data: { ...this.state.data, [name]: target.value },
            errors: { ...this.state.errors, [name]: undefined },
        });
    };
    handleRememberMeChange = event => {
        this.setState({
            rememberMe: !this.state.rememberMe,
        });
    };

    handleSubmit = event => {
        event.preventDefault();

        loginUser(this.state.data).then(res => {
            const data = res.data;
            const { rememberMe } = this.state;
            this.setState({
                errors: [],
                loginFinished: true
            });

            this.props.onLogin(data, rememberMe);
        }).catch(error => {
            console.error(error);
            console.error(error.response.data);

            switch (error.response.status) {
                case 400: {
                    const fieldErrors = error.response.data;
                    Object.keys(fieldErrors).map((fieldName) => {
                        fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                        console.log(fieldErrors[fieldName])
                    });

                    this.setState({
                        errors: fieldErrors
                    });
                    break;
                }
                case 401: {
                    window.location.replace("/login");
                    break;
                }
                default: {
                    console.log("Unexpected error occurred. ", error);
                }
            }
        });
    };

    render() {
        const { errors, loginFinished } = this.state;

        if (loginFinished) {
            return <Redirect to="/"/>
        }

        return (
            <React.Fragment>
                <Container className="col-8" style={{margin: "auto"}}>
                    <h1>Account Log In</h1>
                    <p>
                        Use the form below to log in to your online account.
                        If you are having trouble please use the 'forgot username/password'
                        link or <Link to = "/contact">contact our staff</Link> for assistance.
                    </p>
                </Container>
                <Form className="auth-form" autoComplete="on">
                    <TextInput
                        name="username"
                        labelText="Username"
                        error={errors.username}
                        onInputChange={this.handleInputChange}
                    />
                    <TextInput
                        name="password"
                        labelText="Password"
                        error={errors.password}
                        onInputChange={this.handleInputChange}
                        type="password"
                    />
                    <FormGroup check>
                        <Label check>
                            <Input type="checkbox" onChange={this.handleRememberMeChange}/>{' '}
                            Remember me
                        </Label>
                    </FormGroup>
                    <FormGroup>
                        <Alert color="danger" isOpen={errors.non_field_errors !== undefined}>
                            {errors.non_field_errors}
                        </Alert>
                    </FormGroup>
                    <FormGroup className="auth-other">
                        <Label>
                            <Link to="/auth/reset">Forgot your username or password?</Link>
                        </Label>
                        <br />
                        <Button onClick={(event) => this.handleSubmit(event)}>Submit</Button>
                    </FormGroup>
                </Form>
            </React.Fragment>
        );
    }
}