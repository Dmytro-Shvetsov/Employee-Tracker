import React from 'react';
import axios from 'axios';
import {contactOrganization} from '../../services/mailService';
import {Button, Form, FormGroup, Alert, Container, Label} from 'reactstrap';
import {Input} from '../common/index'
import {Link} from "react-router-dom";


export default class ContactForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            data: {
                email: '',
                subject: '',
                message: '',
            },
            errors: {},
            successMessage: undefined
        };
        this.reqSource = undefined;
        this._isMounted = false;
    }

    setState = (...args) => {
        if (this._isMounted) {
            super.setState(...args);
        }
    };

    handleInputChange = event => {
        const target = event.target;
        const name = target.name;

        this.setState({
            data: { ...this.state.data, [name]: target.value },
            errors: { ...this.state.errors, [name]: undefined },
        });
    };

    handleSubmit = async event => {
        event.preventDefault();

        await this.cancelPreviousRequests();
        const { data } = this.state;
        try {
            const response = await contactOrganization(data, this.reqSource.token);
            console.log('mailed', response.data);
            this.setState({
                data: {},
                errors: [],
                successMessage: response.data.detail
            });
        } catch (error) {
            console.log(error.message);
            if (error.response.status === 400) {
                const fieldErrors = error.response.data;
                Object.keys(fieldErrors).map((fieldName) => {
                    fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                    console.log(fieldErrors[fieldName]);
                });

                this.setState({
                    errors: fieldErrors
                });
            }
        }
    };

    cancelPreviousRequests = async () => {
        if (this.reqSource) {
            this.reqSource.cancel();
        }
        this.reqSource = axios.CancelToken.source();
    };

    componentDidMount() {
        this._isMounted = true;
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
    }

    render() {
        const { data, errors, successMessage } = this.state;

        return (
            <React.Fragment>
                <Container className="col-8" style={{margin: "auto"}}>
                    <h1>Contact Us</h1>
                    <p>
                        Use the form below to contact our staff with any questions you might have.
                        Please fill out the form completely and with as much information as you can — please be detailed.
                        This will provide a quicker and more accurate response from our support team.
                    </p>
                </Container>
                <Form className="form-center" id="contact-form" autoComplete="on">
                    <Input
                        name="email"
                        labelText="Your Email"
                        value={data.email || ""}
                        error={errors.email}
                        onChange={this.handleInputChange}
                    />
                    <Input
                        name="subject"
                        labelText="Subject"
                        value={data.subject || ""}
                        error={errors.subject}
                        onChange={this.handleInputChange}
                    />
                    <Input
                        name="message"
                        labelText="Comments"
                        value={data.message || ""}
                        error={errors.message}
                        onChange={this.handleInputChange}
                        type="textarea"
                    />
                    <FormGroup>
                        <Alert color="danger" isOpen={errors.non_field_errors !== undefined}>
                            {errors.non_field_errors}
                        </Alert>
                    </FormGroup>
                    <FormGroup>
                        <Alert color="success" isOpen={successMessage !== undefined}>
                            {successMessage}
                        </Alert>
                    </FormGroup>
                    <FormGroup>
                        <Button color="secondary" size="lg" onClick={this.handleSubmit}>Submit</Button>
                    </FormGroup>
                </Form>
            </React.Fragment>
        );
    }
}