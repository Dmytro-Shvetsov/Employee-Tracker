import { registerUser } from '../../services/authService.js'
import React from 'react';
import { Button, Form, FormGroup, Label, Input, FormFeedback } from 'reactstrap';
import { Container } from "reactstrap";
import { Link, Redirect } from 'react-router-dom';
import TextInput from '../common/Input'


class RegisterForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            data: {
                username: '',
                email: '',
                password: '',
                password2: '',
                isStaff: false,
            },
            errors: {},
            registrationFinished: false
        };
    }

    handleInputChange = (event) => {
        const target = event.target;
        const name = target.name;

        this.setState({
            data: { ...this.state.data, [name]: target.value},
            errors: {...this.state.errors, [name]: undefined},
        });
    };

    handleIsStaffChange = (event) => {
        this.setState({
            isStaff: !this.state.data.isStaff,
        });
    };

    handleSubmit = (event) => {
        registerUser(this.state.data).then((res) => {
            console.log(res.data);

            const { token } = JSON.parse(res.data);
            this.setState({
                registrationFinished: true
            });
            this.props.onLogin(token);
        }).catch((error) => {
            console.log(error.response.data);

            if (error.response.status === 401) {
                const fieldErrors = error.response.data;

                Object.keys(fieldErrors).map((fieldName) => {
                    fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                });

                this.setState({
                    errors: fieldErrors
                });
            } else {
                console.log("Unexpected error occurred. ", error);
            }
        });
    };

    render() {
        if (this.state.registrationFinished) {
            return <Redirect to="/"/>
        }

        const { errors } = this.state;
        return (
            <React.Fragment>
                <Container className="col-8" style={{margin: "auto"}}>
                    <h1>Account Sign Up</h1>
                    <p>
                        Use the form below to sign in to your online account.
                        Please enter your information exactly as given to you.
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
                        name="email"
                        labelText="Email"
                        error={errors.email}
                        onInputChange={this.handleInputChange}
                    />
                    <TextInput
                        name="password"
                        labelText="Password"
                        error={errors.password}
                        onInputChange={this.handleInputChange}
                        type="password"
                    />
                    <TextInput
                        name="password2"
                        labelText="Confirm password"
                        error={errors.password2}
                        onInputChange={this.handleInputChange}
                        type="password"
                    />
                    <FormGroup check>
                        <Label check>
                            <Input type="checkbox" onChange={this.handleIsStaffChange}/>{' '}
                            I'd like to create a manager account
                        </Label>
                    </FormGroup>
                    <FormGroup className="auth-other">
                        <Button onClick={(event) => this.handleSubmit(event)}>Submit</Button>
                    </FormGroup>
                </Form>
            </React.Fragment>
        );
    }
}

export default RegisterForm;
