import React from 'react';
import {
    Button,
    Form,
    FormGroup,
    Label,
    Alert,
    Container,
    InputGroupAddon,
    InputGroupText,
    InputGroup,
    Spinner
} from 'reactstrap';
import { Input } from '../common/index';
import { getUserProfile } from '../../services/authService';


export default class UserProfile extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            data: {},
            errors: {},
            editing: false
        }
    }

    loadProfileData() {
        const { user } = this.state;
        getUserProfile(user).then(res => {
            this.setState({
                data: JSON.parse(res.data)
            });
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
    }

    componentDidMount() {
        this.loadProfileData();
    }

    handleInputChange = event => {
        const target = event.target;
        const name = target.name;

        this.setState({
            data: { ...this.state.data, [name]: target.value},
            errors: {...this.state.errors, [name]: undefined},
        });
    };

    render() {
        const { user, data, editing } = this.state;
        if (data.id === undefined) {
            return <Spinner/>
        }
        console.log(data);
        return (
            <div className="row">
                    <div className="col-md-12">
                        <div className="card card-primary card-outline">
                            <div className="card-body pt-3">
                                <Form>
                                    <InputGroup>
                                        <Input
                                            disabled={editing}
                                            labelText={"Username"}
                                            value={data.username}
                                            name="username"
                                            placeholder="username"
                                            onInputChange={this.handleInputChange}
                                            required={false}
                                        />

                                    </InputGroup>
                                </Form>
                            </div>

                            <div className="card-footer">
                                <Button className="bg-success float-right">
                                    <i className="far fa-save"></i>
                                    Update Profile
                                </Button>
                            </div>
                        </div>

                </div>
            </div>
        );
    }
};