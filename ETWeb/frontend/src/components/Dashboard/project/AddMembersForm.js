import React from 'react';
import axios from 'axios';
import {CloseCircleOutlined, UserAddOutlined} from '@ant-design/icons';
import {
    Form,
    FormGroup,
    Alert,
    Badge,
    Dropdown,
    DropdownToggle,
    DropdownMenu,
    DropdownItem
} from 'reactstrap';
import {Modal, Input} from '../../common';
import * as utils from '../../../utils';
import * as projectsService from "../../../services/projectsService";


export default class AddMembersForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            windowModalOpen: false,
            matchesDropdownOpen: false,
            username: '',
            searchMembers: [],
            data: {
                projectId: props.projectId,
                selectedMembers: new Set(),
            },
            successMessage: undefined,
            errors: {},
        };
        this.reqSource = undefined;
        this._isMounted = false;
    }

    setState = (...args) => {
        if (this._isMounted) {
            super.setState(...args);
        }
    };

    handleUsernameChange = async event => {
        const username = event.target.value;
        await this.cancelPreviousRequests();
        let searchMembers = [];
        try {
            const response = await projectsService.findMembersByUsername(username, this.reqSource.token);
            searchMembers = JSON.parse(response.data).users;
        } catch (error) {
            if (error.message !== undefined) {
                console.log(error.message);
            }
        }
        // console.log(searchMembers.map(item => item.username));
        this.setState({
            username,
            searchMembers,
            errors: { ...this.state.errors, [name]: undefined },
        });
    };

    windowModalToggle = () => {
        this.setState({
            windowModalOpen: !this.state.windowModalOpen,
            // reset other fields
            matchesDropdownOpen: false,
            username: '',
            searchMembers: [],
            data: {...this.state.data, selectedMembers: new Set()},
            successMessage: undefined,
            errors: {}
        });
    };

    matchesDropdownToggle = () => {
        this.setState({
            matchesDropdownOpen: !this.state.matchesDropdownOpen
        });
    };

    handleMemberSelect = member => {
        // console.log('adding member');
        const prevSelectedUsers = this.state.data.selectedMembers;
        const newSelectedUsers = prevSelectedUsers.copy();
        newSelectedUsers.add(member);
        this.setState({
            data: {...this.state.data, selectedMembers: newSelectedUsers}
        });
    };

    handleMemberDeselect = member => {
        // console.log('removing member');
        const prevSelectedUsers = this.state.data.selectedMembers;
        const newSelectedUsers = prevSelectedUsers.copy();
        newSelectedUsers.delete(member);
        this.setState({
            data: {...this.state.data, selectedMembers: newSelectedUsers}
        });
    };

    cancelPreviousRequests = async () => {
        if (this.reqSource) {
            this.reqSource.cancel();
        }
        this.reqSource = axios.CancelToken.source();
    };

    handleSubmit = async event => {
        event.preventDefault();
        // console.log("Trying to add new members");
        await this.cancelPreviousRequests();
        const { data: { projectId, selectedMembers } } = this.state;
        try {
            const response = await projectsService.addMembersToProject({
                id: projectId,
                new_members: selectedMembers.map(item => item.id)
            }, this.reqSource.token);
            const data = JSON.parse(response.data);
            const formData = this.state.data;
            this.setState({
                successMessage: data.detail,
                username: "",
                searchMembers: [],
                data: {...formData, selectedMembers: new Set()}
            });
            this.props.onMembersAdded(data.project);
            // this.windowModalToggle();
        } catch (error) {
            console.log(error.message);
            if (error.response.status === 400) {
                const fieldErrors = error.response.data;
                Object.keys(fieldErrors).map((fieldName) => {
                    fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                    console.log(fieldErrors[fieldName]);
                });

                this.setState({
                    successMessage: undefined,
                    errors: fieldErrors
                });
            }
        }
    };

    componentDidMount() {
        this._isMounted = true;
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
    }

    renderFoundUsernameMatches () {
        const {searchMembers} = this.state;
        return (
            <React.Fragment>
                {searchMembers.length === 0 ? (
                    <DropdownItem key={-1}>
                        <small className="text-muted m-3" key="hint">No users were found by specified username</small>
                    </DropdownItem>
                ) : (
                    searchMembers.map(user => (
                        <DropdownItem
                            className="d-flex align-items-center"
                            key={user.id}
                            onClick={() => this.handleMemberSelect(user)}
                        >
                            <UserAddOutlined className="ml-2"/>
                            {user.username}
                        </DropdownItem>
                    ))
                )}
            </React.Fragment>
        );
    }

    renderAddedMembers() {
        const {selectedMembers} = this.state.data;
        const children = [];
        selectedMembers.forEach(user => {
            children.push(
                <Badge
                    className="d-flex align-items-center m-1 pl-2 user-badge"
                    style={{height:"1.5rem"}}
                    key={`user-${user.id}`}
                >
                    {user.username}
                    <CloseCircleOutlined key={`deselect-user-${user.id}`} onClick={() => this.handleMemberDeselect(user)}/>
                </Badge>
            );
        });
        // console.log("n-selected", children.length);
        return (
            <React.Fragment>
                {children.length === 0 ? (
                        <div
                            className="d-flex justify-content-center align-items-center"
                            style={{width:"100%", height:"100%"}}
                        >
                            <small className="text-muted">You have not added any members yet.</small>
                        </div>
                    ) : children
                }
            </React.Fragment>
        );
    };

    render() {
        const {windowModalOpen, matchesDropdownOpen, username, successMessage, errors} = this.state;
        // console.log(matchesDropdownOpen);
        return (
            <Modal
                triggerBtnLabel="Add new members"
                modalTitle="Add new members"
                triggerBtnColor="success"
                actionBtnLabel="Save"
                onAction={this.handleSubmit}
                onToggle={this.windowModalToggle}
                modal={windowModalOpen}
            >
                <h6>
                    Start typing username of the account. Click the correct user to add them to the list.
                    Press "Save" to save the changes.
                </h6>
                <Form autoComplete="off">
                    <Dropdown isOpen={matchesDropdownOpen && username !== ""} toggle={this.matchesDropdownToggle} direction="down">
                        <DropdownToggle tag="div">
                            <Input
                                name="username"
                                labelText="Username"
                                error={errors.username}
                                onChange={this.handleUsernameChange}
                            />
                        </DropdownToggle>
                        <DropdownMenu className="matches-dropdown">
                            {this.renderFoundUsernameMatches()}
                        </DropdownMenu>
                    </Dropdown>
                    <div id="new-member-list" className="d-flex flex-wrap align-content-start" color="primary">
                        {this.renderAddedMembers()}
                    </div>
                    <FormGroup className="mt-2">
                        <Alert color="success" isOpen={successMessage !== undefined}>
                            {successMessage}
                        </Alert>
                        <Alert color="danger" isOpen={errors.new_members !== undefined}>
                            {errors.new_members}
                        </Alert>
                    </FormGroup>
                </Form>
            </Modal>
        );
    }
}