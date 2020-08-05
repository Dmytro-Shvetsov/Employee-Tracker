import React from 'react';
import * as projects from '../../services/projectsService'
import { Link } from 'react-router-dom'
import { Spinner } from 'reactstrap'
import { Card, Table, CardHeader, CardFooter, CardBody,
  CardTitle, CardText } from 'reactstrap';
import { NotFound } from '../Pages/index'


export default class ProjectDetail extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
        }
    }

    loadProjectInfo(id) {
        const { user } = this.state;
        projects.getProject(user, id).then(res => {
            console.log("Project loaded.", res.data);
            this.setState({
                ...res.data
            });
        }).catch(error => {
            switch (error.response.status) {
                case 401: {
                    window.location.replace("/login");
                    break;
                }
                case 404: {
                    this.setState({
                        id: null
                    });
                    break;
                }
                default: {
                    console.log("Unexpected error occurred. ", error);
                }
            }
        });
    }

    componentDidMount() {
        const { match: { params } } = this.props;
        this.loadProjectInfo(params.id);
    }

    renderMembersTable() {
        const { user, members } = this.state;
        if (members === undefined) {
            console.error("renderMembersTable() called when information about the project was not loaded");
            return;
        }
        console.log(members);
        return (
            <div>
                <Table>
                    <thead>
                        <tr>
                            <th>#</th>
                            <th>Username</th>
                            <th>Email</th>
                            <th>Account Registration Date</th>
                        </tr>
                    </thead>
                    <tbody>
                        {members.map((m, idx) => (
                            <tr className={m.is_staff ? "staff-user" : ""} key={`${m.id}`}>
                                <th scope="row">{idx + 1}</th>
                                <td>
                                    <Link
                                        to={m.id === user.id ? "/dashboard/account" : `/dashboard/user/${m.id}`}
                                        className="username-link"
                                    >
                                        @{m.username}
                                    </Link>
                                </td>
                                <td>{m.email || <span className="text-secondary">No email provided.</span>}</td>
                                <td>{new Date(Date.parse(m.date_joined)).toLocaleString()}</td>
                            </tr>
                        ))}
                    </tbody>
                </Table>
                <div className="color-explained d-flex">
                    <span className="staff-user disabled d-inline-block" style={{width: "20px", height: "20px"}}/>
                    <span className="d-inline-block small ml-sm-1">- staff users</span>
                </div>
            </div>
        );
    }

    render() {
        const { id, name, description } = this.state;

        if (id === undefined) {
            return <Spinner/>
        }
        if (id === null) {
            return <NotFound/>
        }

        return (
            <Card>
                <CardHeader tag="h3">{name}</CardHeader>
                <CardBody>
                    <CardTitle className="font-italic">Description:</CardTitle>
                    <CardText>{description}</CardText>

                    <CardTitle className="font-italic">Members:</CardTitle>
                    {this.renderMembersTable()}
                </CardBody>
                <CardFooter>Owners: </CardFooter>
            </Card>
        );
    }
};

